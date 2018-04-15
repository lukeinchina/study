package main

import (
	"./btcutil"
	"encoding/json"
	"flag"
	"fmt"
	"log"
	"math/rand"
	"time"
)

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

func upload_chainfor_data(obj *ChainForObj) int {
	if nil == obj {
		return -1
	}
	var succ_count int
	for _, item := range obj.List {
		sec := int(item.CreateDate.Time / 1000)
		url := fmt.Sprintf("https://www.chainfor.com/news/show/%d.html", item.Id)
		btcutil.UploadToServer("chainfor", "链向财经", url, btcutil.GetTitle(item.Introduction), btcutil.GetContent(item.Introduction), sec)
		succ_count += 1
	}
	return succ_count
}

func get_chainfor_data(url string) *ChainForObj {
	bytedata := btcutil.GetHttpData(url)
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
	for i := 0; i < 1; i++ {
		url := fmt.Sprintf("https://www.chainfor.com/news/list/flashnew/data.do?type=0&pageSize=15&pageNo=%d&title=", i+1)
		obj := get_chainfor_data(url)
		total += upload_chainfor_data(obj)
		log.Printf("[%s]\n", url)
	}
	log.Printf("downlaod total [%d]\n", total)
	return total
}

func scan_download() {
	for {
		url := "https://www.chainfor.com/news/list/flashnew/data.do?type=0&pageSize=15&pageNo=1&title="
		obj := get_chainfor_data(url)
		count := upload_chainfor_data(obj)
		log.Printf("scan once [%s], upload count:%d\n", url, count)
		var nsec int = rand.Intn(100) + 10
		time.Sleep(time.Duration(nsec) * time.Second)
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
