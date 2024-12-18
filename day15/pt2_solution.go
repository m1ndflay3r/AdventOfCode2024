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

func allSumOfGPSCoordinates() int {
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
  walls := make(map[string]bool)
  var boxes []Point
  robot := Point{x: 0, y: 0}
  // Initialize walls, boxes, and robot position
  for y := 0; y < height; y++ {
    for x := 0; x < width; x++ {
      switch grid[y][x] {
      case '@':
        robot = Point{x: x * 2, y: y}
      case '#':
        walls[fmt.Sprintf("%d,%d", x*2, y)] = true
        walls[fmt.Sprintf("%d,%d", x*2+1, y)] = true
      case 'O':
        boxes = append(boxes, Point{x: x * 2, y: y})
      }
    }
  }
  // attempt move all boxes
  var moveBox func(collidedBox Point, direction Point, movements *[]map[string]Point) bool
  moveBox = func(collidedBox Point, direction Point, movements *[]map[string]Point) bool {
    nextPositions := []Point{
      {x: collidedBox.x + direction.x, y: collidedBox.y + direction.y},
      {x: collidedBox.x + 1 + direction.x, y: collidedBox.y + direction.y},
    }
    // If collided, stop all movements
    for _, nextPos := range nextPositions {
      if walls[fmt.Sprintf("%d,%d", nextPos.x, nextPos.y)] {
        return false
      }
    }
    // Find collided boxes
    var collidedBoxes []Point
    for _, box := range boxes {
      for _, nextPos := range nextPositions {
        if box != collidedBox && ((box.x == nextPos.x || box.x+1 == nextPos.x) && box.y == nextPos.y) {
          collidedBoxes = append(collidedBoxes, box)
          break
        }
      }
    }
    // If no collided boxes, all movements pass
    if len(collidedBoxes) == 0 {
      return true
    }
    // Check for conflicts
    conflicts := false
    for _, box := range collidedBoxes {
      if moveBox(box, direction, movements) {
        alreadyProcessed := false
        for _, movement := range *movements {
          if movement["box"] == box {
            alreadyProcessed = true
            break
          }
        }
        if !alreadyProcessed {
          *movements = append(*movements, map[string]Point{"box": box, "direction": direction})
        }
      } else {
        conflicts = true
        break
      }
    }
    return !conflicts
  }
  // Process each instruction
  for _, instruction := range instructions {
    direction := DIRECTIONS[instruction]
    position := Point{x: robot.x + direction.x, y: robot.y + direction.y}
    // Only attempt move if no wall present
    if !walls[fmt.Sprintf("%d,%d", position.x, position.y)] {
      var collidedBox *Point
      for i := range boxes {
        if (boxes[i].x == position.x || boxes[i].x+1 == position.x) && boxes[i].y == position.y {
          collidedBox = &boxes[i]
          break
        }
      }
      // If is collided box, attempt move all affected
      if collidedBox != nil {
        var movements []map[string]Point
        if moveBox(*collidedBox, direction, &movements) {
          for _, movement := range movements {
            for i := range boxes {
              if boxes[i].x == movement["box"].x && boxes[i].y == movement["box"].y {
                boxes[i].x += movement["direction"].x
                boxes[i].y += movement["direction"].y
                break
              }
            }
          }
          collidedBox.x += direction.x
          collidedBox.y += direction.y
          robot = position
        }
      } else {
        robot = position
      }
    }
  }
  // Calculate score
  score := 0
  for _, box := range boxes {
    score += box.y*100 + box.x
  }
  fmt.Println(score)
  return score
}

func main() {
  allSumOfGPSCoordinates()
}
