package main

import (
	"./btcutil"
	"encoding/json"
	"flag"
	"fmt"
	"io/ioutil"
	"log"
	"math/rand"
	"net/http"
	"time"
)

/*
{
    "newsflash_id":24775,
     "content":"A股收盘，区块链板块全天在绿盘之下弱势震荡，收跌0.86%。76只概念股中，33只上涨，43只下跌。涨幅前三为：恒银金融（+10.02%），安妮股份（+10.00%），宣亚国际（+3.87%）；跌幅前三为：汇金科技 （-6.48%），佳沃股份（-6.04%），北大荒（-4.98%）。宣亚国际震荡中上涨，由开盘跌幅前三重回收盘时的涨幅前三。",
     "source":"币世界",
     "link_title":"查看详情",
     "link":"",
     "issue_time":1523343941,
     "rank":0,
     "img_path_type":2,
     "bull_vote":19,
     "bad_vote":7,
     "title":"A股收盘：区块链板块两只涨停 整体弱势震荡",
     "voted_type":0,
     "content_length":170
 }
*/
type BSJItem struct {
	Newsflash_id   int    `json:"newsflash_id"`
	Content        string `json:"content"`
	Source         string `json:"source"`
	link_title     string
	link           string
	Issue_time     int `json:"issue_time"`
	rank           int
	img_path_type  int
	bull_vote      int
	bad_vote       int
	Title          string `json:"title"`
	voted_type     int
	content_length int
}

type BSJEntity struct {
	Top    []string
	Buttom []BSJItem `json:"buttom"`
	Date   string
}

type BSJUnreadNum struct {
	Num int `json:"num"`
}

type BSJUnreadMsg struct {
	Error   int          `json:"error"`
	Message string       `json:"message"`
	Data    BSJUnreadNum `json:"data"`
}

type BSJResp struct {
	Error   int         `json:"error"`
	Message string      `json:"message"`
	Data    []BSJEntity `json:"data"`
}

func get_http_data(url string) []byte {
	resp, err := http.Get(url)
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

func get_unread_msg(timestamp int64) *BSJUnreadMsg {
	url := fmt.Sprintf("http://www.bishijie.com/api/news/unreadNum?timestamp=%d", timestamp)
	bytedata := get_http_data(url)
	msg := &BSJUnreadMsg{}
	err := json.Unmarshal(bytedata, msg)
	if err != nil {
		log.Println("unmarshal failed:", err)
		return nil
	}
	return msg
}

func get_bsj_resp(size int) *BSJResp {
	url := fmt.Sprintf("http://www.bishijie.com/api/newsv17/index?size=%d&client=pc", size)
	bytedata := get_http_data(url)
	msg := &BSJResp{}
	err := json.Unmarshal(bytedata, msg)
	if err != nil {
		log.Println("unmarshal failed:", err)
		return nil
	}

	return msg
}

func upload_item(item *BSJItem) bool {
	url := fmt.Sprintf("http://www.bishijie.com/home/newsflashpc/detail?id=%d", item.Newsflash_id)
	btcutil.UploadToServer("bishijie", "币世界", url, item.Title, item.Content, item.Issue_time)
	return true
}

/*
http://www.bishijie.com/api/news/unreadNum?timestamp=1523345731
{"error":0,"message":"","data":{"num":0}}
*/
/*
http://www.bishijie.com/api/newsv17/index?size=4&client=pc
*/

func init_download() {
	timestamp := int64(1523544578)
	for {
		msg := get_unread_msg(timestamp)
		fmt.Printf("error :%d\n", msg.Error)
		fmt.Printf("unread num:%d\n", msg.Data.Num)
		if msg.Error != 0 || msg.Data.Num < 1 {
			log.Printf("error:%d, num:%d\n", msg.Error, msg.Data.Num)
			time.Sleep(60 * time.Second)
			continue
		}
		resp := get_bsj_resp(msg.Data.Num)
		if 0 != resp.Error {
			log.Printf("error:%d\n", msg.Error)
			time.Sleep(30 * time.Second)
			continue
		}
		for _, item := range resp.Data[0].Buttom {
			upload_item(&item)
		}
		log.Printf("[%d] record upload\n", msg.Data.Num)
		break
	}
}

func scan_download() {
	for {
		timestamp := time.Now().Unix() - 600
		msg := get_unread_msg(timestamp)
		fmt.Printf("error :%d\n", msg.Error)
		fmt.Printf("unread num:%d\n", msg.Data.Num)
		if msg.Error != 0 || msg.Data.Num < 1 {
			log.Printf("error:%d, num:%d\n", msg.Error, msg.Data.Num)
			time.Sleep(60 * time.Second)
			continue
		}
		resp := get_bsj_resp(msg.Data.Num)
		if 0 != resp.Error {
			log.Printf("error:%d\n", msg.Error)
			time.Sleep(30 * time.Second)
			continue
		}
		for _, item := range resp.Data[0].Buttom {
			fmt.Printf("[%d][%s]%s\n", item.Issue_time, item.Title, item.Content)
		}
	}
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

/*
[
{
    "top":["04月10日","星期二","今天"],
    "buttom":[
    {"newsflash_id":24781,"content":"在今天下午的博鳌论坛上，中央央行参事、中欧陆家嘴国际金融研究院常务副院长盛松成表示，比特币是区块链的一种应用，如果区块链只用在虚拟货币方面，区块链是没有前途的，虚拟货币本质上不是货币。如果使用区块链，建议不要用在货币上，也不要用在金融上，这是与国家货币政策冲突的。","source":"币世界","link_title":"查看详情","link":"","issue_time":1523345352,"rank":1,"img_path_type":2,"bull_vote":67,"bad_vote":195,"title":"央行参事盛松成：不要将区块链用在货币和金融上，这是没有前途的","voted_type":0,"content_length":132},
    {"newsflash_id":24780,"content":"日本富士通集团中欧地区（瑞士、德国和奥地利）业务负责人Rolf Werner博士宣布加入IOTA团队，他还曾担任富士通科技解决方案公司GmBH董事会主席、T-Systems公司董事会成员及其法国子公司负责人。IOTA现全球均价0.979美元，下跌7.17%。","source":"币世界","link_title":"查看详情","link":"","issue_time":1523345268,"rank":0,"img_path_type":2,"bull_vote":15,"bad_vote":6,"title":"富士通中欧区业务负责人Werner博士加入IOTA","voted_type":0,"content_length":129},
    {"newsflash_id":24776,"content":"智投链IIC与Share X（基于区块链的未上市股权管理和交易平台）达成战略合作，并将在4月13日“第三届中国股权转让论坛”上举行签约仪式，双方将在区块链股权交易等方面进行深入合作。IIC现全球均价0.0029美元，下跌4.93%。","source":"币世界","link_title":"查看详情","link":"","issue_time":1523344171,"rank":0,"img_path_type":2,"bull_vote":27,"bad_vote":6,"title":"智投链IIC与Share X达成战略合作","voted_type":0,"content_length":116},
    {"newsflash_id":24775,"content":"A股收盘，区块链板块全天在绿盘之下弱势震荡，收跌0.86%。76只概念股中，33只上涨，43只下跌。涨幅前三为：恒银金融（+10.02%），安妮股份（+10.00%），宣亚国际（+3.87%）；跌幅前三为：汇金科技 （-6.48%），佳沃股份（-6.04%），北大荒（-4.98%）。宣亚国际震荡中上涨，由开盘跌幅前三重回收盘时的涨幅前三。","source":"币世界","link_title":"查看详情","link":"","issue_time":1523343941,"rank":0,"img_path_type":2,"bull_vote":19,"bad_vote":7,"title":"A股收盘：区块链板块两只涨停 整体弱势震荡","voted_type":0,"content_length":170}],
    "date":"2018-04-10"
}
    ]
*/
