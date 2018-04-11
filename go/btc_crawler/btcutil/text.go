package btcutil

import "strings"

func GetTitle(text string) string {
	s := strings.Index(text, "【")
	e := strings.Index(text, "】")
	if s < 5 && s > -1 && e > s {
		return text[s+3 : e]
	} else {
		return ""
	}
}

func GetContent(text string) string {
	s := strings.Index(text, "【")
	e := strings.Index(text, "】")
	if s < 5 && s > -1 && e > s {
		return text[e+3:]
	} else {
		return text
	}
}
