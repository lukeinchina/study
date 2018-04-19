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

/*
"created_at":
*/
type JinNiuEntity struct {
	Id           int `json:"id"`
	title        *string
	poster_id    *string
	level        int
	Summary      string `json:summary`
	location     *string
	CreatedAt    *string `json:"created_at"`
	updated_time *string
	poster       *string
	tags         []string
	affect       []string
}

type JinNiuData struct {
	Count int            `json:"count"`
	Time  string         `json:"time"`
	List  []JinNiuEntity `json:"list"`
}

type JinNiuResp struct {
	Code    int         `json:"code"`
	Message string      `json:"message"`
	Data    *JinNiuData `json:"data"`
}

/* http://www.jinniu.cn/bitsquawk/infoes?page=1&limit=1 */
func get_jinniu_data(url string) *JinNiuData {
	bytedata := btcutil.GetHttpDataByProxy(url)
	if nil == bytedata {
		return nil
	}

	resp := &JinNiuResp{}
	err := json.Unmarshal(bytedata, resp)
	if err != nil {
		log.Println("unmarshal failed:", err)
		return nil
	}
	if 200 != resp.Code {
		log.Printf("error:%d, %s\n", resp.Code, resp.Message)
		return nil
	}
	return resp.Data
}

func upload_jinniu_data(c redis.Conn, data *JinNiuData) bool {
	for _, item := range data.List {
		tm, e := time.Parse("2006-01-02 15:04:05", *item.CreatedAt)
		if e != nil {
			tm = time.Now()
		}
		url := fmt.Sprintf("http://www.jinniu.cn/kuaixun/id_%d", item.Id)
		if !btcutil.InsertDB(c, url) {
			log.Printf("[%s] exist\n", url)
			continue
		}
		texthash := btcutil.LongestSentenceHash(item.Summary)
		if !btcutil.InsertDB(c, texthash) {
			log.Printf("[%s] exist\n", texthash)
			continue
		}

		btcutil.UploadToServer("jinniu", "金牛财经", url, btcutil.GetTitle(item.Summary), btcutil.GetContent(item.Summary), int(tm.Unix()-8*3600))
		log.Printf("upload %s\n", url)
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

	for idx := 6; idx > 0; idx-- {
		url := fmt.Sprintf("http://www.jinniu.cn/bitsquawk/infoes?page=%d&limit=100", idx)
		log.Printf("%s\n", url)
		data := get_jinniu_data(url)
		if nil == data {
			log.Printf("get [%s] data failed\n", url)
			continue
		}
		upload_jinniu_data(redisc, data)
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
		url := "http://www.jinniu.cn/bitsquawk/infoes?page=1&limit=10"
		data := get_jinniu_data(url)
		if nil == data {
			log.Printf("get [%s] data failed\n", url)
			continue
		}
		upload_jinniu_data(redisc, data)
		time.Sleep(30 * time.Second)
	}
}

func main() {
	var init bool
	flag.BoolVar(&init, "init", false, "initial start down")
	flag.Parse()
	fmt.Printf("init = %t\n", init)

	log.SetPrefix("[jinniu]")
	log.SetFlags(log.Ldate | log.Ltime | log.Lshortfile)
	rand.Seed(time.Now().Unix())
	if init {
		init_download()
	} else {
		scan_download()
	}
}
