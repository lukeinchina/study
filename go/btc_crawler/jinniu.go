package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
)

/*
"created_at":
*/
type JinNiuEntity struct {
	ID           int `json:"id"`
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

	data := &JinNiuResp{}
	body, err := ioutil.ReadAll(resp.Body)
	err = json.Unmarshal(body, data)
	if err != nil {
		fmt.Println("unmarshal failed:", err)
		return
	}
	if 200 != data.Code {
		fmt.Printf("error:%d\n", data.Code)
		return
	}
	for i, v := range data.Data.List {
		fmt.Printf("%d:%s\n", i, v.Summary)
	}
}

func test_json_data() {
	s := ``
	data := &JinNiuResp{}
	err := json.Unmarshal([]byte(s), data)
	fmt.Println(err)
	fmt.Println(data.Code)
	fmt.Println(data.Data.Count)
	fmt.Println(data.Data.Time)
	fmt.Println(data.Data.List[0].Summary)
	fmt.Println(*(data.Data.List[0].CreatedAt))
}

func main() {
	get_json_data("http://www.jinniu.cn/bitsquawk/infoes?page=1&limit=3")
}
