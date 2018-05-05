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

var redis_server string = "127.0.0.1:7788"

type WeilaicaijingEntity struct {
	Id     int    `json:"id"`
	Type   int    `json:"type"`
	Text   string `json:"text"`
	Hour   string `json:"hour"`
	Source string `json:"source"`
	Url    string `json:"url"`
}

type WeilaicaijingData struct {
	Time string                 `json:"time"`
	List []*WeilaicaijingEntity `json:"list"`
}

type WeilaicaijingResp struct {
	Success bool                 `json:"success"`
	Message string               `json:"message"`
	Data    []*WeilaicaijingData `json:"data"`
}

func get_weilaicaijing_data(url string) *WeilaicaijingData {
	bytedata := btcutil.GetHttpDataByProxy(url)
	if bytedata == nil {
		log.Printf("GetHttpDataByProxy nil\n")
		return nil
	}
	resp := &WeilaicaijingResp{}
	err := json.Unmarshal(bytedata, resp)
	if err != nil {
		log.Println("unmarshal failed:", err)
		return nil
	}
	if !resp.Success {
		log.Printf("success:%t, %s\n", resp.Success, resp.Message)
		return nil
	}
	return resp.Data[0]
}

func upload_weilaicaijing_data(c redis.Conn, data *WeilaicaijingData) bool {
	if nil == data {
		return false
	}
	// "2018-04-13 周五"
	date_str := data.Time[:11]
	for idx, item := range data.List {
		url := fmt.Sprintf("http://www.weilaicaijing.com/article/%d", item.Id)
		datetime := date_str + item.Hour
		tm, e := time.Parse("2006-01-02 15:04", datetime)
		if e != nil {
			tm = time.Now()
			log.Printf("parse time failed[%s:%d]\n", url, idx)
		}

		if !btcutil.InsertDB(c, url) {
			log.Printf("[%s] exist\n", url)
			continue
		}
		content := btcutil.GetContent(item.Text)
		texthash := btcutil.LongestSentenceHash(content)
		if !btcutil.InsertDB(c, texthash) {
			log.Printf("[%s] exist\n", texthash)
			continue
		}
		log.Printf("[debug][%s] does not exist[%s]\n", texthash, item.Text)

		btcutil.UploadToServer("weilaicaijing", "未来财经", url, btcutil.GetTitle(item.Text), content, int(tm.Unix()-8*3600))
	}
	return true
}

func init_download() {
	redisc, err := redis.Dial("tcp", redis_server)
	if err != nil {
		fmt.Println("Connect to redis error:", err)
		return
	}
	defer redisc.Close()

	for idx := 100; idx > 0; idx-- {
		url := fmt.Sprintf("http://www.weilaicaijing.com/api/Fastnews/lists?search_str=&page=%d", idx)
		data := get_weilaicaijing_data(url)
		upload_weilaicaijing_data(redisc, data)
		log.Printf("%s download\n", url)
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
		url := "http://www.weilaicaijing.com/api/Fastnews/lists?search_str=&page=1"
		data := get_weilaicaijing_data(url)
		upload_weilaicaijing_data(redisc, data)
		log.Printf("%s download\n", url)
		time.Sleep(38 * time.Second)
	}
}

func main() {
	var init bool
	flag.BoolVar(&init, "init", false, "initial start down")
	flag.Parse()
	fmt.Printf("init = %t\n", init)

	log.SetPrefix("[weilaicaijing]")
	log.SetFlags(log.Ldate | log.Ltime | log.Lshortfile)
	rand.Seed(time.Now().Unix())
	if init {
		init_download()
	} else {
		scan_download()
	}
}
