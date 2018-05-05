package btcutil

import (
	"bytes"
	"crypto/md5"
	"encoding/json"
	"fmt"
	"github.com/garyburd/redigo/redis"
	"io/ioutil"
	"log"
	"net"
	"net/http"
	"net/url"
	"sort"
	"time"
)

func HttpPost(url string, data_type string, data []byte) {
	req, err := http.NewRequest("POST", url, bytes.NewBuffer(data))
	req.Header.Set("X-Custom-Header", "myvalue")
	req.Header.Set("Content-Type", data_type)

	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		log.Printf("post [%s] data failed:%s\n", url, err)
		return
	}
	defer resp.Body.Close()

	log.Println("response Status:", resp.Status)
	_, _ = ioutil.ReadAll(resp.Body)
	///fmt.Println("response Body:", string(body))
}

func get_sign(data map[string]string) string {
	var names []string
	for k, v := range data {
		if v != "" {
			names = append(names, k)
		}
	}
	sort.Strings(names)
	var buf bytes.Buffer
	for idx, name := range names {
		if idx != 0 {
			buf.WriteString("&")
		}
		buf.WriteString(name)
		buf.WriteString("=")
		buf.WriteString(data[name])
	}
	buf.WriteString("BIBAO100_DEV_AUTHENTICATION")
	return fmt.Sprintf("%x", md5.Sum(buf.Bytes()))
}

func UploadToServer(src_key, src_name, src_url, title, content string, post_stamp int) {
	upload_data := map[string]string{
		"source_key":  src_key,
		"source_name": src_name,
		"source_url":  src_url,
		"type":        "0",
		"tags":        "",
	}
	upload_data["sign_timestamp"] = fmt.Sprintf("%d", time.Now().Unix())
	// upload_data["posttime"] = fmt.Sprintf("%d", tm.Unix()-8*3600)
	upload_data["posttime"] = fmt.Sprintf("%d", post_stamp)
	upload_data["subject"] = title
	upload_data["content"] = content

	upload_data["sign"] = get_sign(upload_data)
	json_str, _ := json.Marshal(upload_data)
	HttpPost("http://api-dev.asminsight.com/v1.0/service/news", "application/json", json_str)

	log.Printf("upload [%s][%s]\n", title, content)
}

func dialTimeout(network, addr string) (net.Conn, error) {
	return net.DialTimeout(network, addr, time.Second*30)
}

func GetHttpData(url string) []byte {
	transport := &http.Transport{
		Dial:              dialTimeout,
		DisableKeepAlives: true,
	}

	timeout := time.Duration(5 * time.Second)
	client := &http.Client{Transport: transport, Timeout: timeout}

	resp, err := client.Get(url)
	if err != nil {
		log.Println("http get error:", err)
		return nil
	}
	defer resp.Body.Close()

	body, err := ioutil.ReadAll(resp.Body)
	if nil != err {
		log.Println("Read http body failed:", err)
		return nil
	}
	return body
}

func fetch_proxy_addr() string {
	var redis_server string = "127.0.0.1:7788"
	c, err := redis.Dial("tcp", redis_server)
	if err != nil {
		log.Println("Connect to redis error", err)
		return ""
	}
	defer c.Close()

	proxy_addr, err := redis.String(c.Do("GET", "proxy_addr"))
	if err != nil {
		log.Println("redis get failed:", err)
		return ""
	} else {
		return proxy_addr
	}
}

func GetHttpDataByProxy(url_addr string) []byte {
	proxy := func(_ *http.Request) (*url.URL, error) {
		proxy_addr := fmt.Sprintf("http://%s", fetch_proxy_addr())
		log.Printf("proxy:%s\n", proxy_addr)
		return url.Parse(proxy_addr)
	}

	transport := &http.Transport{
		Proxy:             proxy,
		Dial:              dialTimeout,
		DisableKeepAlives: true,
	}

	timeout := time.Duration(60 * time.Second)

	client := &http.Client{Transport: transport, Timeout: timeout}

	resp, err := client.Get(url_addr)
	if err != nil {
		log.Println("http get error:", err)
		return nil
	}
	defer resp.Body.Close()

	body, err := ioutil.ReadAll(resp.Body)
	if nil != err {
		log.Println("Read http body failed:", err)
		return nil
	}
	return body
}
