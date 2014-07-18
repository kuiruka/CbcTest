all: CbcCalibInst BeFirmware

CbcCalibInst: CbcCalibInst.tex
	pdflatex -shell-escape $< 
	bibtex   $@ 
	pdflatex -shell-escape $< 
	pdflatex -shell-escape $< 
	pdflatex -shell-escape $< 

BeFirmware: BeFirmware.tex
	pdflatex -shell-escape $< 
	pdflatex -shell-escape $< 
	pdflatex -shell-escape $< 
#	bibtex   $@ 
#	pdflatex -shell-escape $< 
	pdflatex -shell-escape $< 
	pdflatex -shell-escape $< 

clean:
	rm -f CbcCalibInst.{aux,lof,log,lot,pdf,toc,bbl,blg}
	rm -f BeFirmware.{aux,lof,log,lot,pdf,toc,bbl,blg}

