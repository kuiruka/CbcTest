all: CbcCalibInst FirmwareHardware

CbcCalibInst: CbcCalibInst.tex
	pdflatex -shell-escape $< 
	bibtex   $@ 
	pdflatex -shell-escape $< 
	pdflatex -shell-escape $< 
	pdflatex -shell-escape $< 

FirmwareHardware: FirmwareHardware.tex
	pdflatex -shell-escape $@ 
	pdflatex -shell-escape $@ 
	pdflatex -shell-escape $@ 
	pdflatex -shell-escape $@ 

clean:
	rm -f CbcCalibInst.{aux,lof,log,lot,pdf,toc,bbl,blg}
	rm -f FirmwareHardware.{aux,lof,log,lot,pdf,toc,bbl,blg}

