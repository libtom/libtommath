all:
	sed -e 's@\.log@-$(VERSION)\.log@g' graphs.dem > graphs-$(VERSION).dem
	gnuplot graphs-$(VERSION).dem

cmp:
	gnuplot before_after.dem

clean:
	rm -f *-*.log *.png graphs-*.dem
