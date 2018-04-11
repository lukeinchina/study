package main

import (
	"./btcutil"
	"encoding/json"
	"flag"
	"fmt"
	"io/ioutil"
	"log"
	"math/rand"
	"net/http"
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

func get_http_data(url string) *ChainForObj {
	resp, err := http.Get(url)
	if err != nil {
		log.Println("http get error:", err)
		return nil
	}
	defer resp.Body.Close()

	body, err := ioutil.ReadAll(resp.Body)
	if nil != err {
		log.Println("Read http body failed:", err)
		return nil
	}
	data := &ChainForResp{}
	err = json.Unmarshal(body, data)
	if err != nil {
		log.Println("unmarshal failed:", err)
		return nil
	}

	if !data.Success {
		log.Println("success == false")
		return nil
	}
	/*
	   for _,v := range data.List {
	       for idx, item := range v.Lives {
	           fmt.Printf("[%d][%s]\n%s\n", idx, get_title(item.Content), get_content(item.Content))
	       }
	   }
	   return data
	*/
	return &data.Obj
}

func upload_data(obj *ChainForObj) int {
	var succ_count int
	for idx, item := range obj.List {
		sec := item.CreateDate.Time / 1000
		fmt.Printf("[%d][%d][%d]\n[%s]\n", idx, item.Id, sec, btcutil.GetTitle(item.Introduction))
		succ_count += 1
	}
	return succ_count
}

func init_download() int {
	var total int
	for i := 0; i < 1; i++ {
		url := fmt.Sprintf("https://www.chainfor.com/news/list/flashnew/data.do?type=0&pageSize=15&pageNo=%d&title=", i+1)
		obj := get_http_data(url)
		total += upload_data(obj)
		log.Printf("[%s]\n", url)
	}
	log.Printf("downlaod total [%d]\n", total)
	return total
}

func scan_download() {
	for {
		url := "https://www.chainfor.com/news/list/flashnew/data.do?type=0&pageSize=15&pageNo=1&title="
		obj := get_http_data(url)
		upload_data(obj)
		log.Printf("scan once [%s]\n", url)
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
