# makes the images... yeah

default:  pses


sliding_window.ps: sliding_window.tif
	tiff2ps -c -e sliding_window.tif > sliding_window.ps
	
expt_state.ps: expt_state.tif
	tiff2ps -c -e expt_state.tif > expt_state.ps

primality.ps: primality.tif
	tiff2ps -c -e primality.tif > primality.ps

sliding_window.pdf: sliding_window.ps
	epstopdf sliding_window.ps
	
expt_state.pdf: expt_state.ps
	epstopdf expt_state.ps

primality.pdf: primality.ps
	epstopdf primality.ps


pses: sliding_window.ps expt_state.ps primality.ps
pdfes: sliding_window.pdf expt_state.pdf primality.pdf

clean:
	rm -rf *.ps *.pdf .xvpics
   