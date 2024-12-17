package main

import (
  "fmt"
  "os"
  "strconv"
  "strings"
)

func main() {
  // Input reading
  data, err := os.ReadFile("input")
  if err != nil {
    fmt.Println("Error reading file:", err)
    return
  }
  // Parse input into robot data
  lines := strings.Split(strings.TrimSpace(string(data)), "\n")
  var robots [][4]int
  for _, line := range lines {
    var robot [4]int
    parts := strings.Split(line, "=")
    // Parse position (p)
    pParts := strings.Split(strings.TrimSpace(parts[1]), ",")
    robot[0] = atoi(strings.TrimSpace(pParts[0]))
    robot[1] = atoi(strings.Split(strings.TrimSpace(pParts[1]), " ")[0])
    // Parse velocity (v)
    vParts := strings.Split(strings.TrimSpace(parts[2]), ",")
    robot[2] = atoi(strings.TrimSpace(vParts[0]))
    robot[3] = atoi(strings.TrimSpace(vParts[1]))
    robots = append(robots, robot)
  }
  // Simulate over time
  for t := 0; t < 10000; t++ {
    nextSet := make(map[[2]int]bool) // Track all positions
    matching := make(map[[2]int]bool)
    // Calculate positions and check for matching robots
    for _, robot := range robots {
      px, py, vx, vy := robot[0], robot[1], robot[2], robot[3]
      xf := mod(px+t*vx, 101)
      yf := mod(py+t*vy, 103)
      // Check if already in nextSet, add to matching
      if nextSet[[2]int{xf, yf}] {
        matching[[2]int{xf, yf}] = true
      }
      // Add neighboring positions
      for dx := -1; dx <= 1; dx++ {
        for dy := -1; dy <= 1; dy++ {
          nextSet[[2]int{xf + dx, yf + dy}] = true
        }
      }
    }
    // Print grid if matching size > 256
    if len(matching) > 256 {
      printGrid(nextSet, 101, 100)
      fmt.Println("t:", t)
      break
    }
  }
}

// safely convert string to int
func atoi(s string) int {
  val, err := strconv.Atoi(s)
  if err != nil {
    fmt.Println("Error converting string to int:", err)
  }
  return val
}

// always return positive results
func mod(a, b int) int {
  result := a % b
  if result < 0 {
    result += b
  }
  return result
}

// Print grid based on positions in nextSet
func printGrid(nextSet map[[2]int]bool, width, height int) {
  for x := 0; x < width; x++ {
    for y := 0; y < height; y++ {
      if nextSet[[2]int{x, y}] {
        fmt.Print("*")
      } else {
        fmt.Print(".")
      }
    }
    fmt.Println()
  }
}
