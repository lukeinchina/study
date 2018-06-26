package main

import (
	"./btcutil"
	"encoding/json"
	"fmt"
	"github.com/garyburd/redigo/redis"
	"log"
	"time"
)

/*
{"ERRORCODE":"0","RESULT":[{"port":"29841","ip":"115.221.125.155"}]}
*/
type ProxyAddr struct {
	Port string `json:"port"`
	Ip   string `json:"ip"`
}

type ProxyResult struct {
	ErrorCode string `json:"ERRORCODE"`
	Result    []*ProxyAddr
}

func fetch_proxy_addr() string {
	url := "http://api.xdaili.cn/xdaili-api//greatRecharge/getGreatIp?spiderId=ecdc482b71774d978301db7865e7fbc7&orderno=YZ20184180556yg1wRv&returnType=2&count=1"

	bytedata := btcutil.GetHttpData(url)
	if bytedata == nil {
		log.Println("get http data failed")
	}
	result := &ProxyResult{}
	err := json.Unmarshal(bytedata, result)
	if err != nil {
		log.Println("unmarshal failed:", err)
		return ""
	}
	if result.ErrorCode != "0" {
		log.Println("errorcode:", result.ErrorCode)
		return ""
	}
	return fmt.Sprintf("%s:%s", result.Result[0].Ip, result.Result[0].Port)
}

func save_to_redis(proxy string) bool {
	var redis_server string = "127.0.0.1:7788"
	redisc, err := redis.Dial("tcp", redis_server)
	if err != nil {
		fmt.Println("Connect to redis error:", err)
		return false
	}
	defer redisc.Close()

	_, err = redisc.Do("SET", "proxy_addr", proxy, "EX", "864000")
	if err != nil {
		log.Println("redis set failed:", err)
	}
	return true
}

func main() {
	for {
		addr := fetch_proxy_addr()
		if "" != addr {
			save_to_redis(addr)
			log.Printf("save proxy:%s\n", addr)
		} else {
			log.Printf("failed save proxy:%s\n", addr)
		}
		time.Sleep(200 * time.Second)
	}
}
