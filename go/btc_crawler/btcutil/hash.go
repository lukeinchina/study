package btcutil

import (
	"crypto/md5"
	"fmt"
	"io"
	"sort"
)

type SentenceSlicee []string

func (s SentenceSlicee) Len() int {
	return len(s)
}
func (s SentenceSlicee) Swap(i, j int) {
	s[i], s[j] = s[j], s[i]
}

func (s SentenceSlicee) Less(i, j int) bool {
	return len(s[i]) > len(s[j])
}

func LongestSentenceHash(text string) string {
	var dst SentenceSlicee
	last := 0
	for i, v := range text {
		if v == '，' || v == '。' || v == '【' || v == '】' || v == '？' || v == '！' || v == '；' {
			if last != i {
				dst = append(dst, text[last:i])
			}
			last = i
		}
	}
	if last < len(text) {
		dst = append(dst, text[last:])
	}
	sort.Sort(dst)
	h := md5.New()
	for idx := 0; idx < 3 && idx < len(dst); idx++ {
		io.WriteString(h, dst[idx])
	}
	return fmt.Sprintf("%x", h.Sum(nil))
}
