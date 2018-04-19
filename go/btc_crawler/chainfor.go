package main

import (
	"./btcutil"
	"encoding/json"
	"flag"
	"fmt"
	"github.com/garyburd/redigo/redis"
	"log"
	"math/rand"
	"time"
)

var redis_server string = "127.0.0.1:6379"

type ChainForFooter struct {
}

type ChainForDate struct {
	Date           int   `json:"date"`
	Day            int   `json:"day"`
	Hours          int   `json:"hours"`
	Minutes        int   `json:"minutes"`
	Month          int   `json:"month"`
	Seconds        int   `json:"seconds"`
	Time           int64 `json:"time"`
	Year           int   `json:"year"`
	timezoneOffset int
}

/*
{
    "createDate": {},
    "id": 18943,
    "introduction": "【KuCoin上线WAN】据KuCoin公告称，目前已上线Wanchain(WAN), 并开放WAN/BTC、WAN/ETH 交易对。",
    "lastUpdateDate": {},
    "relativeDate": "10 分钟前",
    "status": 0,
    "subIntroduction": "",
    "sumCount": 113,
    "timeDate": "20180409",
    "timeDateStr": "2018年4月9日 今天 星期一",
    "title": "",
    "url": "",
    "version": 1,
    "webName": ""
},
*/
type ChainForEntity struct {
	CreateDate      ChainForDate `json:"createDate"`
	Id              int          `json:"id"`
	Introduction    string       `json:"introduction"`
	lastUpdateDate  ChainForDate
	relativeDate    string
	status          int
	subIntroduction string
	sumCount        int
	timeDate        string
	timeDateStr     string
	title           string
	url             string
	version         int
	webName         string
}

/*
{
    "clazz": null,
    "firstPage": true,
    "firstResult": 0,
    "footer": [],
    "lastPage": false,
    "list": [],
    "nextPage": 2,
    "pageNo": 1,
    "pageSize": 15,
    "pagination": false,
    "prePage": 1,
    "total": false,
    "totalCount": 10696,
    "totalFieldName": "",
    "totalPage": 714
}
*/
type ChainForObj struct {
	clazz          *string
	firstPage      bool
	lastPage       bool
	firstResult    int
	footer         []*ChainForFooter
	List           []*ChainForEntity `json:"list"`
	nextPage       int
	pageNo         int
	pageSize       int
	pagination     bool
	prePage        int
	total          bool
	totalCount     int
	totalFieldName string
	totalPage      int
}

type ChainForResp struct {
	msg     string
	Success bool        `json:"success"`
	Obj     ChainForObj `json:"obj"`
}

func upload_chainfor_data(c redis.Conn, obj *ChainForObj) int {
	if nil == obj {
		return -1
	}
	var succ_count int
	for _, item := range obj.List {
		sec := int(item.CreateDate.Time / 1000)
		url := fmt.Sprintf("https://www.chainfor.com/news/show/%d.html", item.Id)
		if !btcutil.InsertDB(c, url) {
			log.Printf("[%s] exist\n", url)
			continue
		}
		texthash := btcutil.LongestSentenceHash(item.Introduction)
		if !btcutil.InsertDB(c, texthash) {
			log.Printf("[%s] exist\n", texthash)
			continue
		}

		btcutil.UploadToServer("chainfor", "链向财经", url, btcutil.GetTitle(item.Introduction), btcutil.GetContent(item.Introduction), sec)
		log.Printf("[%s] upload!", url)
		succ_count += 1
	}
	return succ_count
}

func get_chainfor_data(url string) *ChainForObj {
	bytedata := btcutil.GetHttpDataByProxy(url)
	if nil == bytedata {
		return nil
	}
	data := &ChainForResp{}
	err := json.Unmarshal(bytedata, data)
	if err != nil {
		log.Println("unmarshal failed:", err)
		return nil
	}

	if !data.Success {
		log.Println("success == false")
		return nil
	}
	return &data.Obj
}

func init_download() int {
	var total int

	redisc, err := redis.Dial("tcp", redis_server)
	if err != nil {
		fmt.Println("Connect to redis error:", err)
		return 0
	}
	defer redisc.Close()

	for i := 0; i < 5; i++ {
		url := fmt.Sprintf("https://www.chainfor.com/news/list/flashnew/data.do?type=0&pageSize=15&pageNo=%d&title=", i+1)
		obj := get_chainfor_data(url)
		if obj == nil {
			continue
		}
		total += upload_chainfor_data(redisc, obj)
		log.Printf("[%s]\n", url)
	}
	log.Printf("downlaod total [%d]\n", total)
	return total
}

func scan_download() {
	for {
		url := "https://www.chainfor.com/news/list/flashnew/data.do?type=0&pageSize=15&pageNo=1&title="
		obj := get_chainfor_data(url)
		if obj == nil {
			log.Println("get_chainfor_data nil. sleep 30 for next try")
			time.Sleep(30 * time.Second)
			continue
		}

		redisc, err := redis.Dial("tcp", redis_server)
		if err != nil {
			log.Println("Connect to redis error:", err)
			time.Sleep(10 * time.Second)
			continue
		}

		count := upload_chainfor_data(redisc, obj)
		var nsec int = rand.Intn(30) + 10
		log.Printf("scan once [%s], upload count:%d, sleep %d seconds\n", url, count, nsec)
		time.Sleep(time.Duration(nsec) * time.Second)
		redisc.Close()
	}
}

func main() {
	var init bool
	flag.BoolVar(&init, "init", false, "initial start down")
	flag.Parse()
	fmt.Println("parsed? = ", flag.Parsed())
	fmt.Printf("init = %t\n", init)

	log.SetPrefix("[chainfor]")
	log.SetFlags(log.Ldate | log.Ltime | log.Lshortfile)
	rand.Seed(time.Now().Unix())
	if init {
		init_download()
	} else {
		scan_download()
	}
}
