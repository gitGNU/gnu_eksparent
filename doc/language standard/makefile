#This makefile requires latexmk

NAME := doc
PDFTOOL := evince
#enable xelatex
#remember to remove inputenc
XELATEX:=-e '$$pdflatex=q/xelatex %O -interaction=nonstopmode -halt-on-error %S/'


all:
	latexmk -pdf $(XELATEX) $(NAME).tex
	
run: all
	$(PDFTOOL) $(NAME).pdf
	
clean:
	rm $(NAME).pdf
	
cleanall:
	rm $(NAME).aux
	rm $(NAME).fdb_latexmk
	rm $(NAME).fls
	rm $(NAME).log
	rm $(NAME).pdf
