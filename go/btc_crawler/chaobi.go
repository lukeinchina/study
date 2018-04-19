package main

import (
	"./btcutil"
	"encoding/json"
	"flag"
	"fmt"
	"github.com/garyburd/redigo/redis"
	"log"
	"math/rand"
	"strings"
	"time"
)

var redis_server string = "127.0.0.1:6379"

/*
{"updatedTime":"2018-04-15 20:25:30","id":5669,"newsContent":"员工美店也是国美希望搭建的一个新渠道。”</p>","newsLevel":5,"praiseNum":0,"trampleNum":0
*/
type ChaoBiEntity struct {
	UpdatedTime string `json:"updatedTime"`
	Id          int    `json:"id"`
	Content     string `json:"newsContent"`
	newsLevel   int
	praiseNum   int
	trampleNum  int
}

type ChaoBiResp struct {
	Status string          `json:"status"`
	Code   string          `json:"code"`
	Msg    string          `json:"msg"`
	Result []*ChaoBiEntity `json:"result"`
	data   int
}

func get_time_stamp(timestr string) int {
	tm, e := time.Parse("2006-01-02 15:04:05", timestr)
	if e != nil {
		log.Println(e.Error())
		tm = time.Now()
	}
	return int(tm.Unix() - 8*3600)
}

func upload_chaobi_data(c redis.Conn, item *ChaoBiEntity) bool {
	url := fmt.Sprintf("http://www.chaobi.com/%d.html", item.Id) // fake

	item.Content = strings.Replace(item.Content, "<p>", "", -1)
	item.Content = strings.Replace(item.Content, "</p>", "", -1)

	if !btcutil.InsertDB(c, url) {
		log.Printf("[%s] exist\n", url)
		return false
	}
	texthash := btcutil.LongestSentenceHash(item.Content)
	if !btcutil.InsertDB(c, texthash) {
		log.Printf("[%s] exist\n", texthash)
		return false
	}
	btcutil.UploadToServer("chaobi", "炒币网", url, "", item.Content, get_time_stamp(item.UpdatedTime))
	log.Printf("[%s] upload!", url)
	return true
}

func init_download() {
	redisc, err := redis.Dial("tcp", redis_server)
	if err != nil {
		fmt.Println("Connect to redis error:", err)
		return
	}
	defer redisc.Close()

	for i := 1; i < 5; i++ {
		url := fmt.Sprintf("http://rest.chaobi.com/sscbnewsinfo/getNewSmallInfos.json?currentPage=%d&pageSize=20", i)
		bytedata := btcutil.GetHttpDataByProxy(url)
		if nil == bytedata {
			log.Println("GetHttpDataByProxy nil", err)
			continue
		}
		resp := &ChaoBiResp{}
		err := json.Unmarshal(bytedata, resp)
		if err != nil {
			log.Println("unmarshal failed:", err)
			break
		}
		if resp.Status != "0" || resp.Code != "000" {
			log.Printf("[error] [%s] [%s]\n", resp.Msg, url)
			break
		}
		for _, item := range resp.Result {
			upload_chaobi_data(redisc, item)
		}
		log.Printf("finished [%s]\n", url)
	}
}

func scan_download() {
	redisc, err := redis.Dial("tcp", redis_server)
	if err != nil {
		fmt.Println("Connect to redis error:", err)
		return
	}
	defer redisc.Close()

	for {
		url := "http://rest.chaobi.com/sscbnewsinfo/getNewSmallInfos.json?currentPage=1&pageSize=5"
		bytedata := btcutil.GetHttpDataByProxy(url)
		if nil == bytedata {
			log.Println("GetHttpDataByProxy nil")
			time.Sleep(30 * time.Second)
			continue
		}

		resp := &ChaoBiResp{}
		err := json.Unmarshal(bytedata, resp)
		if err != nil {
			log.Println("unmarshal failed:", err)
			time.Sleep(30 * time.Second)
			continue
		}
		if resp.Status != "0" || resp.Code != "000" {
			log.Printf("[error] [%s] [%s]\n", resp.Msg, url)
			time.Sleep(30 * time.Second)
			continue
		}
		for _, item := range resp.Result {
			upload_chaobi_data(redisc, item)
		}
		n := rand.Intn(30) + 10
		time.Sleep(time.Duration(n) * time.Second)
	}
}

func main() {
	var init bool
	flag.BoolVar(&init, "init", false, "initial start down")
	flag.Parse()
	fmt.Printf("init = %t\n", init)

	log.SetPrefix("[chaobi]")
	log.SetFlags(log.Ldate | log.Ltime | log.Lshortfile)

	if init {
		init_download()
	} else {
		scan_download()
	}
}
