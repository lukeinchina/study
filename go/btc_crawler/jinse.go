package main

import (
	//	"./util"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"strings"
	"time"
)

type JinSeItem struct {
	Id         int    `json:"id"`
	Content    string `json:"content"`
	CreatedAt  int    `json:"created_at"`
	UpCounts   int    `json:"up_counts"`
	DownCounts int    `json:"down_counts"`
	ZanStatus  string `json:"zan_status"`
	readings   *string
}

type JinSeEntity struct {
	date  string
	Lives []JinSeItem `json:"lives"`
}

type JinSeResp struct {
	News     int           `json:"news"`
	Count    int           `json:"count"`
	TopId    int           `json:"top_id"`
	BottomId int           `json:"bottom_id"`
	List     []JinSeEntity `json:"list"`
}

func get_title(text string) string {
	s := strings.Index(text, "【")
	e := strings.Index(text, "】")
	if s < 5 && s > -1 && e > s {
		return text[s+3 : e]
	} else {
		return ""
	}
}

func get_content(text string) string {
	s := strings.Index(text, "【")
	e := strings.Index(text, "】")
	if s < 5 && s > -1 && e > s {
		return text[e+3:]
	} else {
		return text
	}
}

func upload_item(items []JinSeItem, idx int) {
	item := items[idx]
	fmt.Printf("[%d][%s]%s\n", idx, get_title(item.Content), get_content(item.Content))
	UploadToServer()
}

func upload_data(res *JinSeResp) {
	for i := 0; i < res.News; i++ {
		upload_item(res.List[0].Lives, i)
	}
}

func get_http_data(url string) *JinSeResp {
	resp, err := http.Get(url)
	if err != nil {
		fmt.Println("http get error:", err)
		return nil
	}
	defer resp.Body.Close()

	data := &JinSeResp{}
	body, err := ioutil.ReadAll(resp.Body)
	err = json.Unmarshal(body, data)
	if err != nil {
		fmt.Println("unmarshal failed:", err)
		return nil
	}

	fmt.Printf("[%s] news[%d]\n", url, data.News)
	/*
	   for _,v := range data.List {
	       for idx, item := range v.Lives {
	           fmt.Printf("[%d][%s]\n%s\n", idx, get_title(item.Content), get_content(item.Content))
	       }
	   }
	*/
	return data
}

func scan_jinse_news() {
	res := get_http_data("https://api.jinse.com/v3/live/list?limit=5")
	if nil == res {
		log.Panic("get_http_data empty")
		return
	}
	log.Printf("top_id:%d, bottom_id:%d\n", res.TopId, res.BottomId)
	upload_data(res)

	for {
		url := fmt.Sprintf("https://api.jinse.com/v3/live/list?limit=20&flag=up&id=%d", res.TopId)
		res = get_http_data(url)
		if res.News < 1 {
			log.Printf("no new data from %d\n", res.TopId)
		} else {
			upload_data(res)
		}
		time.Sleep(50 * time.Second)
	}
}

func init_jinse_news() {
	res := get_http_data("https://api.jinse.com/v3/live/list?limit=20&flag=up&id=10000")
	if nil == res {
		log.Panic("get_http_data empty")
		return
	}
	log.Printf("top_id:%d, bottom_id:%d\n", res.TopId, res.BottomId)
	upload_data(res)

	for {
		url := fmt.Sprintf("https://api.jinse.com/v3/live/list?limit=20&flag=up&id=%d", res.TopId)
		res = get_http_data(url)
		if res.News < 1 {
			log.Printf("no new data from %d\n", res.TopId)
			break
		}
		upload_data(res)
		time.Sleep(50 * time.Second)
	}
}

func main() {
	log.SetPrefix("[JINSE]")
	log.SetFlags(log.Ldate | log.Ltime | log.Lshortfile)
	init_jinse_news()
}

/* "news":0,
{
"news":2,
"count":2,
"top_id":21528,
"bottom_id":21527,
"list":[
    {
      "date":"2018-04-09",
         "lives":[
            {"id":21528,"content":"","created_at":1523259533,"up_counts":35,"down_counts":6,"zan_status":"","readings":[]},
            {"id":21527,"content":"","created_at":1523259148,"up_counts":32,"down_counts":30,"zan_status":"","readings":[]}
         ]
    }]
}
*/
