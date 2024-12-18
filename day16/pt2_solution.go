package main

import (
  "bufio"
  "fmt"
  "math"
  "os"
)

type Point struct {
  x, y int
}

const (
  BIG_N = 1024
  MAX   = math.MaxInt32
)

var (
  tab       [BIG_N][BIG_N]rune
  boolMap   [BIG_N][BIG_N]bool
  costs     [BIG_N][BIG_N][4]int
  sx, sy    int
  n, m      int
  turns     = [4]Point{{-1, 0}, {0, -1}, {1, 0}, {0, 1}}
  turnCosts = [4]int{0, 1000, 2000, 1000}
)

func countPath(x, y, turn, cost int) {
  for i := 0; i < 4; i++ {
    if i == 2 {
      continue
    }
    txX := turns[(turn+i)%4].x
    txY := turns[(turn+i)%4].y
    if tab[y+txY][x+txX] == '.' {
      newCost := cost + 1 + turnCosts[i]
      if newCost < costs[y+txY][x+txX][(turn+i)%4] {
        costs[y+txY][x+txX][(turn+i)%4] = newCost
        countPath(x+txX, y+txY, (turn+i)%4, newCost)
      }
    }
  }
}

func resolvePath(ex, ey, vec int) {
  boolMap[ey][ex] = true
  if ex == sx && ey == sy {
    return
  }
  ex2, ey2 := 0, 0
  min := MAX
  for i := 0; i < 4; i++ {
    if costs[ey][ex][i] < min {
      min = costs[ey][ex][i]
    }
  }
  for i := 0; i < 4; i++ {
    if costs[ey][ex][i] == min || (costs[ey][ex][i] == min+1000 && vec == i) {
      ex2 = ex + turns[(i+2)%4].x
      ey2 = ey + turns[(i+2)%4].y
      resolvePath(ex2, ey2, i)
    }
  }
}

func main() {
  file, err := os.Open("input")
  if err != nil {
    panic(err)
  }
  defer file.Close()
  scanner := bufio.NewScanner(file)
  var lines []string
  for scanner.Scan() {
    lines = append(lines, scanner.Text())
  }
  if err := scanner.Err(); err != nil {
    panic(err)
  }
  n = len(lines)
  for i := 0; i < n; i++ {
    for j, ch := range lines[i] {
      tab[i][j] = ch
    }
  }
  ex, ey := 0, 0
  m = len(lines[0])
  for i := 0; i < n; i++ {
    for j := 0; j < m; j++ {
      for k := 0; k < 4; k++ {
        costs[i][j][k] = MAX
      }
      switch tab[i][j] {
      case 'S':
        sx, sy = j, i
        tab[i][j] = '.'
      case 'E':
        ex, ey = j, i
        tab[i][j] = '.'
      }
    }
  }
  countPath(sx, sy, 2, 0)
  resolvePath(ex, ey, 0)
  cnt := 0
  for i := 0; i < n; i++ {
    for j := 0; j < m; j++ {
      if boolMap[i][j] {
        cnt++
      }
    }
  }
  fmt.Println(cnt)
}
