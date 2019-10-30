package main

import (
	"fmt"

	"github.com/common-nighthawk/go-figure"

	"log"

	"github.com/jacobsa/go-serial/serial"
)

func main() {
	// bulbhead
	// poison

	myFigure := figure.NewFigure("Halloween", "poison", true)
	myFigure.Print()
	fmt.Println()
	fmt.Println()
	myFigure = figure.NewFigure("Ghost Fire", "poison", true)
	myFigure.Print()
	fmt.Println()
	fmt.Println()
	fmt.Println()
	figure.NewFigure("When a Ghost turns Red ", "doom", true).Print()
	figure.NewFigure("you can shoot it", "doom", true).Print()
	figure.NewFigure("Press the start button", "doom", true).Print()
	//	figure.NewFigure("Give your reasons", "doom", true).Blink(10000, 500, -1)

	// Set up options.
	options := serial.OpenOptions{
		PortName:        "/dev/cu.usbmodem144401",
		BaudRate:        115200,
		DataBits:        8,
		StopBits:        1,
		MinimumReadSize: 4,
	}

	// Open the port.
	port, err := serial.Open(options)
	if err != nil {
		log.Fatalf("serial.Open: %v", err)
	}

	// Make sure to close it later.
	defer port.Close()

	// // Write 4 bytes to the port.
	// b := []byte{0x00, 0x01, 0x02, 0x03}
	// n, err := port.Write(b)
	// if err != nil {
	// 	log.Fatalf("port.Write: %v", err)
	// }
	// fmt.Println("Wrote", n, "bytes.")

	for {
		buf := make([]byte, 1)
		n, err := port.Read(buf)
		if err != nil {
			log.Fatal(err)
		}
		ar := fmt.Sprintf("%s", buf[:n])

		fmt.Println("GOT COMMAND :" + ar)
		if ar == "E" {
			b := []byte("NG")
			n, err := port.Write(b)
			if err != nil {
				log.Fatalf("port.Write: %v", err)
			}
			fmt.Println("Wrote", n, "bytes.")

		}
		if ar == "R" {
			fmt.Println("Round End")
		}

	}
}
