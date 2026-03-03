# compile
puzzle: puzzle.cpp
	g++ -std=c++11 -o puzzle puzzle.cpp

clean:
	rm -f puzzle
	rm -rf ./results ./jgraphs

folders:
	mkdir -p results
	mkdir -p jgraphs

# generate the 5 jgraphs
cabin: puzzle folders
	@echo "Generating cabin puzzle"
	./puzzle eps_files/cabin.eps 8 10 > jgraphs/cabin.jgr
	jgraph -P jgraphs/cabin.jgr | ps2pdf - | convert -density 300 - -quality 100 results/cabin_puzzle.jpg

kitten: puzzle folders
	@echo "Generating kitten puzzle"
	./puzzle eps_files/kitten.eps 5 8 > jgraphs/kitten.jgr
	jgraph -P jgraphs/kitten.jgr | ps2pdf - | convert -density 300 - -quality 100 results/kitten_puzzle.jpg

lighthouse: puzzle folders
	@echo "Generating lighthouse puzzle"
	./puzzle eps_files/lighthouse.eps 14 10 > jgraphs/lighthouse.jgr
	jgraph -P jgraphs/lighthouse.jgr | ps2pdf - | convert -density 300 - -quality 100 results/lighthouse_puzzle.jpg

marshlands: puzzle folders
	@echo "Generating marshlands puzzle"
	./puzzle eps_files/marshlands.eps 11 11 > jgraphs/marshlands.jgr
	jgraph -P jgraphs/marshlands.jgr | ps2pdf - | convert -density 300 - -quality 100 results/marshlands_puzzle.jpg

north_ireland: puzzle folders
	@echo "Generating north_ireland puzzle"
	./puzzle eps_files/north_ireland.eps 8 10 > jgraphs/north_ireland.jgr
	jgraph -P jgraphs/north_ireland.jgr | ps2pdf - | convert -density 300 - -quality 100 results/north_ireland_puzzle.jpg

# run all
all: cabin kitten lighthouse marshlands north_ireland 
