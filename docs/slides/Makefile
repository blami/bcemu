all: bt_balazond_slides.pdf

# Build DVI document (latex):
bt_balazond_slides.dvi: bt_balazond_slides.tex
	latex bt_balazond_slides.tex
#	latex bt_balazond_slides.tex

# Build PS document (dvi2ps):
bt_balazond_slides.ps: bt_balazond_slides.dvi
	dvips $< -o bt_balazond_slides.ps

# Build PDF document (latex + ps2pdf):
bt_balazond_slides.pdf: bt_balazond_slides.ps
	ps2pdf $<

# Clean-up build directory
clean:
	-rm -f *.log *.aux *.bbl *.blg *.lof *.lot *.dvi *.toc *.out *~ *.ps *.pdf *.nav *.snm *.vrb
	-rm -f fig/*.aux
