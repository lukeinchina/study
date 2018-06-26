package main

import (
	"./btcutil"
	"bytes"
	"crypto/md5"
	"encoding/json"
	"fmt"
	"github.com/PuerkitoBio/goquery"
	"github.com/garyburd/redigo/redis"
	"io/ioutil"
	"log"
	"net/http"
	"regexp"
	"sort"
	"strconv"
	"strings"
	"time"
)

var redis_server string = "127.0.0.1:7788"

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
			}
			texthash := btcutil.LongestSentenceHash(content)
			if !btcutil.InsertDB(redisc, texthash) {
				log.Printf("[%s] exist\n", texthash)
				return
			}

			tm, e := time.Parse("2006-01-02 15:04:05", real_date)
			if e != nil {
				log.Println(e.Error())
				tm = time.Now()
			}

			UploadToServer("btc123", "比特币123", href, title, content, tm)
			log.Printf("upload[%s]%s\n", title, content)
		})
		log.Printf("succ:%s\n", url)
		time.Sleep(time.Second * 41)
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
http://www.8btc.com/news
=> http://www.8btc.com/news/page/1
OK
*/
/* s: 5分钟前 */
func extract_timestamp(s string) int64 {
	stamp := time.Now().Unix()
	fmt.Printf("[%d]\n", stamp)

	text := strings.TrimSpace(s)
	pat := `^(\d+)(\W+)前`
	reg := regexp.MustCompile(pat)
	params := reg.FindStringSubmatch(text)
	if len(params) != 3 {
		log.Printf("[%s] get timestamp failed\n", s)
		return stamp
	}
	d, err := strconv.Atoi(params[1])
	if err != nil {
		log.Printf(" get timestamp int faild [%s] [%s]\n", params[1], s)
		return stamp
	}
	unit_str := params[2]
	var unit int64

	switch unit_str {
	case "分钟":
		unit = 60
		break
	case "小时":
		unit = 3600
		break
	case "天":
		unit = 86400
		break
	case "周":
		unit = 604800
		break
	case "月":
		unit = 30 * 86400
		break
	default:
		log.Printf("get unit failed for [%s] [%s]\n", unit_str, s)
		break
	}
	stamp -= int64(d) * unit
	return stamp
}

func Get8BTC() {
	url := "http://www.8btc.com/news/page/1"
	doc, err := goquery.NewDocument(url)
	if err != nil {
		log.Fatal(err)
	}
	doc.Find(".article-content").Each(func(i int, s *goquery.Selection) {
		ts := s.Find(".article-title a")
		href, err1 := ts.Attr("href")
		title, err2 := ts.Attr("title")
		if !err1 || !err2 {
			return
		}
		content := s.Find(".fast_news_content").Text()

		if "" == content || title == "" || href == "" {
			log.Printf("title or content text empty [%s]\n", url)
			return
		}
		infos := s.Find(".article-info")
		timestr := infos.Find("span").First().Text()
		if "" == timestr {
			log.Printf("get time text failed [%s]\n", url)
			return
		}
		fmt.Printf("[%d][%s][%s]:%s\n%s\n", i, timestr, href, title, content)
	})
}

func GetChainDDDetail(url string) string {
	doc, err := goquery.NewDocument(url)
	if err != nil {
		log.Fatal(err)
	}
	text := ""
	doc.Find(".container .post-container article p").Each(func(i int, s *goquery.Selection) {
		detail := s.Text()
		text = strings.Replace(detail, "【链得得播报】", "", -1)
	})
	return text
}

/*
 */
func GetChainDD() {
	redisc, err := redis.Dial("tcp", redis_server)
	if err != nil {
		fmt.Println("Connect to redis error", err)
		return
	}
	defer redisc.Close()

	for {
		url := fmt.Sprintf("http://www.chaindd.com/nictation/1")
		doc, err := goquery.NewDocument(url)
		if err != nil {
			log.Fatal(err)
		}
		doc.Find(".container .day_part ul").Each(func(i int, s *goquery.Selection) {
			s.Find("li").Each(func(idx int, ins *goquery.Selection) {
				title_select := ins.Find("h2 a")
				title := title_select.Text()
				href, e := title_select.Attr("href")
				if title == "" || href == "" {
					return
				}
				if !e {
					log.Printf("get href faild\n")
					return
				}

				titlehash := btcutil.GetMD5Hash(title)
				if !btcutil.InsertDB(redisc, titlehash) {
					log.Printf("[%s] exist\n", titlehash)
					return
				}
				content := GetChainDDDetail(href)

				cur := time.Now()
				time_select := ins.Find(".info time")
				tstr := time_select.Text()
				time_str := fmt.Sprintf("%d-%02d-%02d %s", cur.Year(), cur.Month(), cur.Day(), tstr)
				tm, err := time.Parse("2006-01-02 15:04", time_str)
				if err != nil {
					fmt.Println(err.Error())
					tm = time.Now()
				}

				texthash := btcutil.LongestSentenceHash(content)
				if !btcutil.InsertDB(redisc, texthash) {
					log.Printf("[%s] exist\n", texthash)
					return
				}
				UploadToServer("chaindd", "链得得", href, title, content, tm)
				log.Printf("[%s][%s][%s]\n", url, title, content)

			})
		})

		log.Printf("scan:%s\n", url)
		time.Sleep(time.Second * 39)
	}
}

func main() {
	log.SetPrefix("[BTC]")
	log.SetFlags(log.Ldate | log.Lshortfile)
	// GetHelloChain()
	// c := make(chan int)

	GetChainDD()
	//GetChainDDDetail("http://www.chaindd.com/nictation/3082111.html")
	// GetBtc123News()
	// Get8BTC()
	// x := <-c
	// fmt.Printf("%d\n", x)
}
