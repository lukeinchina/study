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
	bytedata := btcutil.GetHttpData(url)
	if bytedata == nil {
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

func upload_weilaicaijing_data(data *WeilaicaijingData) int {
	if nil == data {
		return 0
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
		btcutil.UploadToServer("weilaicaijing", "未来财经", url, btcutil.GetTitle(item.Text), btcutil.GetContent(item.Text), int(tm.Unix()-8*3600))
	}
	return 0
}

func init_download() {
	for idx := 100; idx > 0; idx-- {
		url := fmt.Sprintf("http://www.weilaicaijing.com/api/Fastnews/lists?search_str=&page=%d", idx)
		data := get_weilaicaijing_data(url)
		upload_weilaicaijing_data(data)
		log.Printf("%s download\n", url)
	}
}

func scan_download() {
	for {
		url := "http://www.weilaicaijing.com/api/Fastnews/lists?search_str=&page=1"
		data := get_weilaicaijing_data(url)
		upload_weilaicaijing_data(data)
		log.Printf("%s download\n", url)
		time.Sleep(108 * time.Second)
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
