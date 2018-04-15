#!/bin/sh

for f in bishijie.go chainfor.go chaobi.go crawler.go jinniu.go jinse.go weilaicaijing.go
do
    go build $f
    test $? && mv ${f%.*} bin
done
