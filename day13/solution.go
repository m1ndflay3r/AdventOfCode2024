package main

import (
  "fmt"
  "os"
  "regexp"
  "strconv"
  "strings"
)

func mincost(ax, ay, bx, by, px, py int) int {
  // Calculate b and brem
  b := (ay*px - ax*py) / (ay*bx - ax*by)
  brem := (ay*px - ax*py) % (ay*bx - ax*by)
  //calculate a and arem
  a := (px - b*bx) / ax
  arem := (px - b*bx) % ax
  // If arem or brem is not zero, return 0, else return a*3 + b
  if arem != 0 || brem != 0 {
    return 0
  }
  return a*3 + b
}

func main() {
  // Read input from stdin
  input, _ := os.ReadFile(os.Stdin.Name())
  // Split input into separate machine blocks
  machinesRaw := strings.Split(string(input), "\n\n")
  // Regex to find integers in each machine string
  re := regexp.MustCompile(`\d+`)
  // Parse machines into a slice of tuples
  var machines [][6]int
  for _, machine := range machinesRaw {
    matches := re.FindAllString(machine, -1)
  if len(matches) == 6 {
    var machine [6]int
    for i, match := range matches {
      machine[i], _ = strconv.Atoi(match)
    }
  machines = append(machines, machine)
  }
}

  // Calculate sum of mincosts for all machines
var total int
  for _, m := range machines {
    total += mincost(m[0], m[1], m[2], m[3], m[4], m[5])
  }
  fmt.Println(total)
  // Base value
  base := 10000000000000
  // Calculate sum for part 2 with adjusted base
  var totalPart2 int
  for _, m := range machines {
    totalPart2 += mincost(m[0], m[1], m[2], m[3], base+m[4], base+m[5])
  }
  fmt.Println(totalPart2)
}
