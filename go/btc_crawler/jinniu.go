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

/*
"created_at":
*/
type JinNiuEntity struct {
	Id          int     `json:"id"`
	Title       *string `json:"title"`
	Content     string  `json:"content"`
	IsImportant int     `json:"isImportant"`
	CreatedAt   int64   `json:"createdAt"`
}

type JinNiuDataInfo struct {
	curPage  int
	pageNum  int
	totalNum int
	maxTime  int64
}

type JinNiuData struct {
	info *JinNiuDataInfo
	List []JinNiuEntity `json:"list"`
}

/*
{"code":200,"msg":"查询成功","sysTime":1525485383487,"data":
{"info":{"curPage":1,"pageNum":1,"totalNum":5413,"maxTime":1525485383471},
"list":[{"id":7236,"title":null,"content":"【阿里财报披露：过去一年，蚂蚁金服坚定区块链等科技的战略性布局】5月4日，阿里巴巴公布2018财年业绩。财报显示，在过去的一个财年，蚂蚁金服不仅在支付、风控等底层技术上自主研发并掌握核心知识产权，对于区块链等科技也坚定战略性布局。目前，蚂蚁区块链团队拥有的技术专利申请数量，在全球互联网公司中排列第一。","isImportant":0,"createdAt":1525484531000}]}
}
*/
type JinNiuResp struct {
	Code    int         `json:"code"`
	Message string      `json:"message"`
	Data    *JinNiuData `json:"data"`
	sysTime int
}

/* http://www.jinniu.cn/bitsquawk/infoes?page=1&limit=1 */
func get_jinniu_data(url string) *JinNiuData {
	bytedata := btcutil.GetHttpDataByProxy(url)
	// bytedata := btcutil.GetHttpData(url)
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
	log.Printf("%s\n", string(bytedata[:]))
	return resp.Data
}

func upload_jinniu_data(c redis.Conn, data *JinNiuData) bool {
	for _, item := range data.List {
		stamp := int(item.CreatedAt / 1000)
		url := fmt.Sprintf("http://www.jinniu.cn/kuaixun/id_%d", item.Id)
		if !btcutil.InsertDB(c, url) {
			log.Printf("[%s] exist\n", url)
			continue
		}
		content := btcutil.GetContent(item.Content)
		texthash := btcutil.LongestSentenceHash(content)
		if !btcutil.InsertDB(c, texthash) {
			log.Printf("[%s] exist\n", texthash)
			continue
		}
		log.Printf("[debug][%s] does not exist[%s]\n", texthash, content)

		btcutil.UploadToServer("jinniu", "金牛财经", url, btcutil.GetTitle(item.Content), content, stamp)
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
		// url := "http://www.jinniu.cn/bitsquawk/infoes?page=1&limit=10"
		url := "http://www.jinniu.cn/v1/news?page=1&pageNum=1"
		data := get_jinniu_data(url)
		if nil == data {
			log.Printf("get [%s] data failed\n", url)
			time.Sleep(18 * time.Second)
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
