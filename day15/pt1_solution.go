package main

import (
  "bufio"
  "fmt"
  "os"
  "strings"
)

type Point struct {
  x int
  y int
}

var DIRECTIONS = map[rune]Point{
  '^': {x: 0, y: -1},
  '>': {x: 1, y: 0},
  'v': {x: 0, y: 1},
  '<': {x: -1, y: 0},
}

func sumOfGPSCoordinates() int {
  file, err := os.Open("./input")
  if err != nil {
    panic(err)
  }
  defer file.Close()
  scanner := bufio.NewScanner(file)
  var lines []string
  for scanner.Scan() {
    lines = append(lines, scanner.Text())
  }
  parts := strings.Split(strings.Join(lines, "\n"), "\n\n")
  gridLines := strings.Split(parts[0], "\n")
  instructions := strings.Join(strings.Split(parts[1], "\n"), "")
  grid := make([][]rune, len(gridLines))
  for i, line := range gridLines {
    grid[i] = []rune(line)
  }
  width := len(grid[0])
  height := len(grid)
  var moveBox func(position, direction Point) bool
  moveBox = func(position, direction Point) bool {
    nextPos := Point{x: position.x + direction.x, y: position.y + direction.y}
    if grid[nextPos.y][nextPos.x] == '.' {
      grid[position.y][position.x], grid[nextPos.y][nextPos.x] = grid[nextPos.y][nextPos.x], grid[position.y][position.x]
      return true
    } else if grid[nextPos.y][nextPos.x] == '#' {
      return false
    } else {
      if moveBox(nextPos, direction) {
        grid[position.y][position.x], grid[nextPos.y][nextPos.x] = grid[nextPos.y][nextPos.x], grid[position.y][position.x]
        return true
      }
    }
    return false
  }
  robot := Point{x: 0, y: 0}
  for y := 0; y < height; y++ {
    for x := 0; x < width; x++ {
      if grid[y][x] == '@' {
        robot = Point{x: x, y: y}
        grid[y][x] = '.'
      }
    }
  }
  for _, instruction := range instructions {
    direction := DIRECTIONS[instruction]
    position := Point{x: robot.x + direction.x, y: robot.y + direction.y}
    if grid[position.y][position.x] != '#' {
      if grid[position.y][position.x] == '.' {
        robot = position
      } else if grid[position.y][position.x] == 'O' && moveBox(position, direction) {
        robot = position
      }
    }
  }
  score := 0
  for y := 0; y < height; y++ {
    for x := 0; x < width; x++ {
      if grid[y][x] == 'O' {
        score += y*100 + x
      }
    }
  }
  fmt.Println(score)
  return score
}

func main() {
  sumOfGPSCoordinates()
}
