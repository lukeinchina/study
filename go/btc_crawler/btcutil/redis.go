package btcutil

import "log"
import "github.com/garyburd/redigo/redis"

func InsertDB(c redis.Conn, key string) bool {
	is_key_exist, err := redis.Bool(c.Do("EXISTS", key))
	if err != nil {
		log.Println("error:", err)
		return false
	} else if is_key_exist {
		log.Printf("[%s] exist in redis\n", key)
		return false
	} else {
		_, err = c.Do("SET", key, 1, "EX", "1000000000")
		if err != nil {
			log.Println("redis set failed:", err)
		}
		return true
	}
}
