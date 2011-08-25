all: bt_balazond.pdf

# Build DVI document (latex):
bt_balazond.dvi: bt_balazond.tex ch/*.tex bt_balazond.bib
	latex bt_balazond.tex
	bibtex bt_balazond
	latex bt_balazond.tex
	# re-run latex to rebuild links
	latex bt_balazond.tex

# Build PS document (dvi2ps):
bt_balazond.ps: bt_balazond.dvi
	dvips $< -o bt_balazond.ps

# Build PDF document (latex + ps2pdf):
bt_balazond.pdf: bt_balazond.ps
	ps2pdf $<

# Clean-up build directory
clean:
	-rm -f *.log *.aux *.bbl *.blg *.lof *.lot *.dvi *.toc *.out *~ *.ps *.pdf
	-rm -f ch/*.aux
	-rm -f fig/*.aux
