#! /usr/bin/Rscript

args <- commandArgs(trailingOnly = TRUE)
tab = read.table(args[1])
id = args[2]

fn =paste("data/hist-",id, ".pdf", sep="") 

pdf(fn)
hist(tab[,1], breaks="FD")
bla = dev.off()
cat(paste("created hist file " , fn, "\n",sep=""))
