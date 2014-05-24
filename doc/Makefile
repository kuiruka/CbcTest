all: CbcCalibInst

CbcCalibInst: CbcCalibInst_MR.tex
	pdflatex -shell-escape $@ 
	bibtex   $@ 
	pdflatex -shell-escape $@ 
	pdflatex -shell-escape $@ 
	pdflatex -shell-escape $@ 

clean:
	rm -f CbcCalibInst.{aux,lof,log,lot,pdf,toc,bbl,blg}

