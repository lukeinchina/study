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

var redis_server string = "127.0.0.1:7788"

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

func upload_item(c redis.Conn, item *JinSeItem) bool {
	url := fmt.Sprintf("https://www.jinse.com/lives/%d.htm", item.Id)

	if !btcutil.InsertDB(c, url) {
		// log.Printf("[%s] exist\n", url)
		return false
	}
	content := btcutil.GetContent(item.Content)
	texthash := btcutil.LongestSentenceHash(content)
	if !btcutil.InsertDB(c, texthash) {
		log.Printf("[%s] exist\n", texthash)
		return false
	}
	log.Printf("[debug][%s] does not exist[%s]\n", texthash, content)

	btcutil.UploadToServer("jinse", "金色财经", url, get_title(item.Content), content, item.CreatedAt)
	return true
}

func upload_data(c redis.Conn, res *JinSeResp) {
	for i := 0; i < res.News; i++ {
		for _, item := range res.List[0].Lives {
			upload_item(c, &item)
		}
	}
}

func get_jinse_resp(url string) *JinSeResp {
	bytedata := btcutil.GetHttpDataByProxy(url)
	if bytedata == nil {
		log.Println("GetHttpDataByProxy nil")
		return nil
	}

	data := &JinSeResp{}
	err := json.Unmarshal(bytedata, data)
	if err != nil {
		log.Println("unmarshal failed:", err)
		return nil
	}

	return data
}

func scan_jinse_news() {

	redisc, err := redis.Dial("tcp", redis_server)
	if err != nil {
		log.Println("Connect to redis error:", err)
		return
	}
	defer redisc.Close()

	for {
		url := "https://api.jinse.com/v3/live/list?limit=3"
		res := get_jinse_resp(url)
		if nil == res {
			log.Printf("nil get_jinse_esp\n")
			time.Sleep(33 * time.Second)
			continue
		}
		if res.News < 1 {
			log.Printf("no new data from\n")
		} else {
			upload_data(redisc, res)
		}
		time.Sleep(28 * time.Second)
	}
}

func init_jinse_news() {
	redisc, err := redis.Dial("tcp", redis_server)
	if err != nil {
		log.Println("Connect to redis error:", err)
		return
	}
	defer redisc.Close()

	res := get_jinse_resp("https://api.jinse.com/v3/live/list?limit=20&flag=up&id=10000")
	if nil == res {
		log.Panic("get_jinse_resp empty")
		return
	}
	log.Printf("top_id:%d, bottom_id:%d\n", res.TopId, res.BottomId)
	upload_data(redisc, res)

	for {
		url := fmt.Sprintf("https://api.jinse.com/v3/live/list?limit=20&flag=up&id=%d", res.TopId)
		res = get_jinse_resp(url)
		if res.News < 1 {
			log.Printf("no new data from %d\n", res.TopId)
			break
		}
		upload_data(redisc, res)
		time.Sleep(25 * time.Second)
	}
}

func main() {
	var init bool
	flag.BoolVar(&init, "init", false, "initial start down")
	flag.Parse()
	fmt.Printf("init = %t\n", init)

	log.SetPrefix("[JINSE]")
	log.SetFlags(log.Ldate | log.Ltime | log.Lshortfile)
	rand.Seed(time.Now().Unix())
	if init {
		init_jinse_news()
	} else {
		scan_jinse_news()
	}

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
