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
  // Initialize quadrant counts
  quadCounts := [2][2]int{}
  // Process each robot
  for _, robot := range robots {
    px, py, vx, vy := robot[0], robot[1], robot[2], robot[3]
    xf := mod(px+100*vx, 101)
    yf := mod(py+100*vy, 103)
    // Ignore boundaries
    if xf == 50 || yf == 51 {
      continue
    }
    // Determine quadrant
    xq := 0
    yq := 0
    if xf < 50 {
      xq = 1
    }
    if yf < 51 {
      yq = 1
    }
    quadCounts[xq][yq]++
  }
  // Calculate result
  ans := quadCounts[0][0] * quadCounts[0][1] * quadCounts[1][0] * quadCounts[1][1]
  fmt.Println("Result:", ans)
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
