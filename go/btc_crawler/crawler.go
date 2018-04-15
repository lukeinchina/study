package main

import (
	"bytes"
	"crypto/md5"
	"encoding/json"
	"fmt"
	"github.com/PuerkitoBio/goquery"
	"github.com/garyburd/redigo/redis"
	"io/ioutil"
	"log"
	"net/http"
	"sort"
	"strings"
	"time"
)

var redis_server string = "127.0.0.1:6379"

func GetTextSimHash(data string) string {
	return fmt.Sprintf("%x", md5.Sum([]byte(data)))
}

func InsertDB(c redis.Conn, key string) bool {
	is_key_exist, err := redis.Bool(c.Do("EXISTS", key))
	if err != nil {
		log.Println("error:", err)
		return false
	} else if is_key_exist {
		log.Printf("[%s] exist. continue\n", key)
		return false
	} else {
		_, err = c.Do("SET", key, 1, "EX", "1000000000")
		if err != nil {
			log.Println("redis set failed:", err)
		}
		return true
	}
}

func UploadToServer(src_key string, src_name, src_url string, title string, content string, tm time.Time) {
	upload_data := map[string]string{
		"source_key":  src_key,
		"source_name": src_name,
		"source_url":  src_url,
		"type":        "0",
		"tags":        "",
	}
	upload_data["sign_timestamp"] = fmt.Sprintf("%d", time.Now().Unix())
	upload_data["posttime"] = fmt.Sprintf("%d", tm.Unix()-8*3600)
	upload_data["subject"] = title
	upload_data["content"] = content

	upload_data["sign"] = GetSign(upload_data)
	json_str, _ := json.Marshal(upload_data)
	HttpPost("http://api-dev.asminsight.com/v1.0/service/news", "application/json", json_str)
}

func GetSign(data map[string]string) string {
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
	// fmt.Println("response Headers:", resp.Header)
	_, _ = ioutil.ReadAll(resp.Body)
	//fmt.Println("response Body:", string(body))

}

func GetJsonData(url string) string {
	resp, err := http.Get(url)
	if err != nil {
		// handle error
	}

	defer resp.Body.Close()
	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		// handle error
	}

	fmt.Println(string(body))
	return string(body)
}

/*
https://www.chainfo.com/live/list.html
http://www.jinse.com/lives
http://news.btc123.com/news
http://www.bishijie.com/kuaixun
http://www.hellochain.info/lives
http://kx.jgy.com
http://www.chaobi.com/page/lives/index.html
http://www.jinniu.cm/kuaixun   =====
http://www.weilaicaijing.com/NowExpress
http://www.8btc.com/news
http://www.gongxiangcj.com/short_news

https://www.cailianpress.com/index
https://api.xuangubao.com/api/pc/msgs?subjids=138&isHotSubject=true?limit=50
http://kuaixun.eastmoney.com
http://news.10jqka.com.cn/realtimenews.html
http://live.sina.com.cn/zt/fv/finance/globalnews1
*/

/*
http://www.bishijie.com/kuaixun   JS
================
*/

/*
链向财经:https://www.chainfor.com/live/list.html
*/
func GetChainfor() {
	doc, err := goquery.NewDocument("https://www.chainfor.com/live/list.html")
	if err != nil {
		log.Fatal(err)
	}
	// Find the review items
	doc.Find(".kuaixun-list-box").Each(func(i int, s *goquery.Selection) {
		// title_sele := s.Find("k-text")
		title := s.Text()
		fmt.Printf("[%d]: %s \n", i, title)
	})
}

/*
金色财经:http://www.jinse.com/lives
*/
func GetJinSe() {
	doc, err := goquery.NewDocument("http://www.jinse.com/lives")
	if err != nil {
		log.Fatal(err)
	}
	// Find the review items
	//doc.Find(".con-item .live-info").Each(func(i int, s *goquery.Selection) {
	doc.Find(".js-left .js-lives").Each(func(i int, s *goquery.Selection) {
		title := s.Text()
		// href, _ := sele.Attr("href")
		// fmt.Printf("[%d]: %s \n href: %s\n", i, title, href)
		fmt.Printf("[%d]: %s \n href: n", i, title)
	})
}

func GetBtcNewsContent(url string) string {
	/* http://news.btc123.com/news/detail?id=12828 */
	var buf bytes.Buffer
	doc, err := goquery.NewDocument(url)
	if err != nil {
		log.Fatal(err)
	}
	doc.Find("div#n_dtcontn .n_left .n_dtcont").Each(func(i int, s *goquery.Selection) {
		s.Find("div:first-child").Contents().Each(func(j int, ins *goquery.Selection) {
			_, flag := ins.Attr("style")
			if !flag {
				if ins.Text() == "" {
					buf.WriteString("<br>")
				} else {
					buf.WriteString(strings.TrimSpace(ins.Text()))
				}
			}
		})
	})

	return buf.String()
}

/*
BTC123:http://news.btc123.com/news
*/
func GetBtc123News() {

	redisc, err := redis.Dial("tcp", redis_server)
	if err != nil {
		fmt.Println("Connect to redis error", err)
		return
	}
	defer redisc.Close()

	for {
		url := fmt.Sprintf("http://news.btc123.com/news?page=%d", 1)
		doc, err := goquery.NewDocument(url)
		if err != nil {
			log.Fatal(err)
		}

		// Find the review items
		doc.Find(".wrapfull .n_newscontent").Each(func(i int, s *goquery.Selection) {
			// For each item found, get the band and title
			var content string
			title_select := s.Find(".n_newstitle a")
			title, exist := title_select.Attr("title")
			if !exist {
				log.Printf("get title string failed\n")
			}
			href, _ := title_select.Attr("href")
			if strings.HasPrefix(href, "/news") {
				href = "http://news.btc123.com" + href
			}
			/* 已经存在了 */
			if !InsertDB(redisc, href) {
				return
			}
			if strings.HasPrefix(href, "http://news.btc123.com") {
				content = GetBtcNewsContent(href)
			}

			date_sel := s.Find(".n_newsdate")
			date_str := strings.TrimSpace(date_sel.Text())
			real_date := date_str[0 : len(date_str)-2]

			/* 如果是站外的，没有抓，用摘要代替 */
			if content == "" {
				content = strings.TrimSpace(s.Find(".n_newsnote").Text())
				// log.Printf("[%s]%s\n", content)
			}

			tm, e := time.Parse("2006-01-02 15:04:05", real_date)
			if e != nil {
				log.Println(e.Error())
				tm = time.Now()
			}

			UploadToServer("btc123", "比特币123", href, title, content, tm)
			log.Printf("POST:%s[%d]\n", url, i)
		})
		log.Printf("succ:%s\n", url)
		break
		time.Sleep(time.Second * 40)
	}
}

/* OK
http://www.hellochain.info/lives-html
*/
func GetHelloChain() {
	doc, err := goquery.NewDocument("http://www.hellochain.info/lives")
	if err != nil {
		log.Fatal(err)
	}
	// Find the review items
	doc.Find(".content .lives-list p").Each(func(i int, s *goquery.Selection) {
		title := s.Text()
		href, _ := s.Find("a").Attr("href")
		fmt.Printf("[%d]: %s \n%s\n", i, title, href)
	})
}

/*
OK
*/
func GetJGY() {
	doc, err := goquery.NewDocument("http://kx.jgy.com/Default_1")
	if err != nil {
		log.Fatal(err)
	}
	// Find the review items
	doc.Find("body form .content div div h3").Each(func(i int, s *goquery.Selection) {
		a := s.Next().Find("a")
		title := a.Text()
		href, _ := a.Attr("href")
		if strings.HasPrefix(href, "Kuaixun") {
			href = "http://kx.jgy.com/" + href
		}
		fmt.Printf("[%d]: %s \nhref:%s\n", i, title, href)
	})
}

/*
JS. ==========
*/
func GetChaoBi() {
	doc, err := goquery.NewDocument("http://www.chaobi.com/page/lives/index.html")
	if err != nil {
		log.Fatal(err)
	}
	doc.Find(".content .new-wrap").Each(func(i int, s *goquery.Selection) {
		title := s.Text()
		fmt.Printf("[%d]:%s\n", i, title)
	})
}

/*
JS ========
*/
func GetWeiLaiCaiJing() {
	doc, err := goquery.NewDocument("http://www.weilaicaijing.com/NowExpress")
	if err != nil {
		log.Fatal(err)
	}
	doc.Find("body").Each(func(i int, s *goquery.Selection) {
		title := s.Text()
		fmt.Printf("[%d]:%s\n", i, title)
	})
}

/*
http://www.8btc.com/news
=> http://www.8btc.com/news/page/1
OK
*/
func Get8BTC() {
	doc, err := goquery.NewDocument("http://www.8btc.com/news/page/1")
	if err != nil {
		log.Fatal(err)
	}
	doc.Find(".article-content").Each(func(i int, s *goquery.Selection) {
		ts := s.Find(".article-title a")
		title := ts.Text()
		content := s.Find(".fast_news_content").Text()
		fmt.Printf("[%d]:%s\n%s\n", i, title, content)
	})
}

/*
http://www.gongxiangcj.com/short_news
http://www.gongxiangcj.com/short_news?page=2
1131page
*/
func GetGongXiangCJ() {
	redisc, err := redis.Dial("tcp", redis_server)
	if err != nil {
		fmt.Println("Connect to redis error", err)
		return
	}
	defer redisc.Close()

	for {
		url := fmt.Sprintf("http://www.gongxiangcj.com/short_news?page=%d", 1)
		doc, err := goquery.NewDocument(url)
		if err != nil {
			log.Fatal(err)
		}
		doc.Find(".kx-left-kuai-new").Each(func(i int, s *goquery.Selection) {
			times := s.Find(".kx-left-kuai-new-date")
			time_str := strings.TrimSpace(times.Text())
			content := s.Find(".kx-left-kuai-new-con p").Text()
			title := ""
			j := strings.Index(content, "【")
			k := strings.Index(content, "】")
			if j >= 0 && k > j {
				title = content[j+3 : k]
				content = content[k+3:]
			}
			tm, e := time.Parse("2006-01-02 15:04", time_str)
			if e != nil {
				fmt.Println(e.Error())
				tm = time.Now()
			}

			hashkey := GetTextSimHash(content)
			if InsertDB(redisc, hashkey) {
				UploadToServer("gongxiangcj", "共享财经", "http://www.gongxiangcj.com/short_news", title, content, tm)
				log.Printf("%s:%d POST\n", url, i)
			}
		})

		log.Printf("scan:%s\n", url)
		break
		time.Sleep(time.Second * 60)
	}
}

func main() {
	log.SetPrefix("[BTC]")
	log.SetFlags(log.Ldate | log.Lshortfile)
	// GetHelloChain()
	// c := make(chan int)

	// GetGongXiangCJ()
	// GetChaoBi()
	// Get8BTC()
	GetWeiLaiCaiJing()
	// GetBtc123News()
	// GetChainfor()
	// GetJsonData("https://www.chainfor.com/news/list/flashnew/data.do?type=0&pageSize=15&pageNo=1&title=")

	// x := <-c
	// fmt.Printf("%d\n", x)
}
