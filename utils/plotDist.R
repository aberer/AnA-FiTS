#! /usr/bin/Rscript

args <- commandArgs(trailingOnly = TRUE)

if(length(args) < 2)
  {
    cat("USAGE: script <table> <id>\n")
    quit("no")
  }

tab = read.table(args[1])
id = args[2]


fn =paste("data/hist-",id, ".pdf", sep="") 

pdf(fn)
hist(tab[,1])
cat(paste("mean ", mean(tab[,1]), "\n"))
bla = dev.off()
## cat(paste("created hist file " , fn, "\n",sep=""))
