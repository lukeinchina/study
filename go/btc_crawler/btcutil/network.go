package btcutil

import (
	"bytes"
	"crypto/md5"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
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
		panic(err)
	}
	defer resp.Body.Close()

	fmt.Println("response Status:", resp.Status)
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
}

func GetHttpData(url string) []byte {
	resp, err := http.Get(url)
	if err != nil {
		fmt.Println("http get error:", err)
		return nil
	}
	defer resp.Body.Close()

	body, err := ioutil.ReadAll(resp.Body)
	if nil != err {
		fmt.Println("Read http body failed:", err)
		return nil
	}
	return body
}
