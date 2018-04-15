package main

import (
	"./btcutil"
	"flag"
	//	"encoding/json"
	"fmt"
	"log"
)

type ChaoBiResp struct {
	Status string `json:"status"`
	Code   string `json:"code"`
	Msg    string `json:"msg"`
	// "result"
}

func get_chaobi_resp(url string) *ChaoBiResp {
	bytedata := btcutil.GetHttpData(url)
	fmt.Printf("%s\n", string(bytedata))
	return nil
}

func init_download() {
	for i := 1; i < 2; i++ {
		url := fmt.Sprintf("http://rest.chaobi.com/sscbnewsinfo/getNewSmallInfos.json?currentPage=%d&pageSize=20", i)
		log.Printf("%s\n", url)
	}
}

func scan_download() {
	url := "http://rest.chaobi.com/sscbnewsinfo/getNewSmallInfos.json?currentPage=1&pageSize=2"
	get_chaobi_resp(url)
}

func main() {
	var init bool
	flag.BoolVar(&init, "init", false, "initial start down")
	flag.Parse()
	fmt.Printf("init = %t\n", init)

	log.SetPrefix("[bishijie]")
	log.SetFlags(log.Ldate | log.Ltime | log.Lshortfile)
	// rand.Seed(time.Now().Unix())
	if init {
		init_download()
	} else {
		scan_download()
	}
}
