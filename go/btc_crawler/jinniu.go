package main

import (
	"./btcutil"
	"encoding/json"
	"flag"
	"fmt"
	"log"
	"math/rand"
	"net/http"
	"time"
)

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
func get_json_data(url string) {
	resp, err := http.Get(url)
	if err != nil {
		fmt.Println("http get error:", err)
		return
	}
	defer resp.Body.Close()

}

func get_jinniu_data(url string) *JinNiuData {
	bytedata := btcutil.GetHttpData(url)
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

func upload_jinniu_data(data *JinNiuData) bool {
	for _, v := range data.List {
		tm, e := time.Parse("2006-01-02 15:04:05", *v.CreatedAt)
		if e != nil {
			tm = time.Now()
		}
		url := fmt.Sprintf("http://www.jinniu.cn/kuaixun/id_%d", v.Id)
		btcutil.UploadToServer("jinniu", "金牛财经", url, btcutil.GetTitle(v.Summary), btcutil.GetContent(v.Summary), int(tm.Unix()-8*3600))
		log.Printf("upload %s\n", url)
	}
	return true
}

func init_download() {
	for idx := 6; idx > 0; idx-- {
		url := fmt.Sprintf("http://www.jinniu.cn/bitsquawk/infoes?page=%d&limit=100", idx)
		log.Printf("%s\n", url)
		data := get_jinniu_data(url)
		if nil == data {
			log.Printf("get [%s] data failed\n", url)
			continue
		}
		upload_jinniu_data(data)
	}
	// get_json_data("http://www.jinniu.cn/bitsquawk/infoes?page=1&limit=3")
}

func scan_download() {
}

func main() {
	var init bool
	flag.BoolVar(&init, "init", false, "initial start down")
	flag.Parse()
	fmt.Printf("init = %t\n", init)

	log.SetPrefix("[bishijie]")
	log.SetFlags(log.Ldate | log.Ltime | log.Lshortfile)
	rand.Seed(time.Now().Unix())
	if init {
		init_download()
	} else {
		scan_download()
	}
}
