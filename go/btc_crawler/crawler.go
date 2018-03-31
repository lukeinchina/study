package main

import (
	"bytes"
	"crypto/md5"
	"encoding/json"
	"fmt"
	"github.com/PuerkitoBio/goquery"
	"io/ioutil"
	"log"
	"net/http"
	"sort"
	"strings"
	"time"
)

func GetSign(data map[string]string) string {
	var names []string
	for k, v := range data {
		if v != "" {
			names = append(names, k)
		}
	}
	sort.Strings(names)
	var buf bytes.Buffer
	for _, name := range names {
		buf.WriteString(name)
		buf.WriteString("=")
		buf.WriteString(data[name])
		buf.WriteString("&")
	}
	buf.WriteString("key=BIBAO100_DEV_AUTHENTICATION")
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
	fmt.Println("response Headers:", resp.Header)
	body, _ := ioutil.ReadAll(resp.Body)
	fmt.Println("response Body:", string(body))

}

/*
https://www.chainfo.com/live/list.html
http://www.jinse.com/lives
http://news.btc123.com/news
http://www.bishijie.com/kuaixun
http://www.hellochain.info/lives
http://kx.jgy.com
http://www.chaobi.com/page/lives/index.html
http://www.jinniu.com/kuaixun   =====
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
	doc.Find(".content").Each(func(i int, s *goquery.Selection) {
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
	// div class="live-date"
	// div class="live-info"
	doc.Find(".con-item .live-info").Each(func(i int, s *goquery.Selection) {
		sele := s.Find("a")
		title := sele.Text()
		href, _ := sele.Attr("href")
		fmt.Printf("[%d]: %s \n href: %s\n", i, title, href)
	})
}

/*
BTC123:http://news.btc123.com/news
*/
func GetBtc123News() {

	doc, err := goquery.NewDocument("http://news.btc123.com/news")
	if err != nil {
		log.Fatal(err)
	}

	// Find the review items
	doc.Find(".wrapfull .n_newscontent").Each(func(i int, s *goquery.Selection) {
		// For each item found, get the band and title
		title_select := s.Find(".n_newstitle a")
		title := title_select.Text()
		href, _ := title_select.Attr("href")
		if strings.HasPrefix(href, "/news") {
			href = "http://new.btc123.com" + href
		}
		summary := strings.TrimSpace(s.Find(".n_newsnote").Text())
		text_href, _ := s.Find(".n_newsnote a").Attr("href")
		fmt.Printf("[%d]: %s \n href: %s\n", i, title, href)
		fmt.Printf("summary %s text href:%s\n", summary, text_href)
	})
}

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

func GetJGY() {
	doc, err := goquery.NewDocument("http://kx.jgy.com")
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
	doc.Find(".content .new-wrap .clearfix").Each(func(i int, s *goquery.Selection) {
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
	doc, err := goquery.NewDocument("http://www.gongxiangcj.com/short_news?page=2")
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

		upload_data := map[string]string{
			"source_key":  "gongxiangcj",
			"source_name": "共享财经",
			"source_url":  "http://www.gongxiangcj.com/short_news",
			"type":        "0",
		}
		upload_data["sign_timestamp"] = fmt.Sprintf("%d", time.Now().Unix())
		upload_data["posttime"] = fmt.Sprintf("%d", tm.Unix())
		upload_data["subject"] = title
		upload_data["content"] = content

		upload_data["sign"] = GetSign(upload_data)
		json_str, _ := json.Marshal(upload_data)
		if i == 0 {
			fmt.Printf("%s", json_str)
			HttpPost("http://39.104.108.104:10000/", "application/json", json_str)
		}
	})
}

func main() {
	GetGongXiangCJ()
	/*
		data := map[string]string{
			"title":   "hello,world",
			"content": "hello，世界",
			"other":   "anything else",
		}
		str, _ := json.Marshal(data)
		fmt.Printf("%s\n", GetSign(data))
		fmt.Printf("%s\n", str)
	*/
}
