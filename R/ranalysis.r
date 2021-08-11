library(reshape2)
library(ggplot2)
library(scales)
library(readr)
library(tidyverse)
setwd("C:/Users/user/Desktop/IFDxpersonality/IFD_activity/IFD_activity")


strID <- "Evol22"

str1 <- paste0(strID,"activities")
data <- read.table(paste0(str1, ".txt"), sep="\t", header = F)

df <- (t(subset(data, select = -c(V1, V1002))))
colnames(df) <- data$V1

length(df[1,])

minwv = 0.0;     # minimal (weight) value
maxwv = 1.0;     # maximal (weight) value
steps = 101;  # num. of bins across range
stepsize = (maxwv - minwv)/steps  # bin range size


#Frequency matrix:

mtrxwP1 <- matrix(nrow = steps, ncol = length(df[1,]), dimnames=list(seq(minwv+stepsize, maxwv, stepsize)))  # Frequency matrix
vecFpos <- vector()

for (i in 1:length(df[1,])){
  
  vecFpos[i] = length(which(df[,i] > 0.05))/length(df[,1])
  
  mtrxwP1[,i] = table(cut(df[,i], seq(minwv, maxwv, stepsize), right=T))/1000
  
}

colnames(mtrxwP1) <- data$V1


P_act <- ggplot(data = melt(t(mtrxwP1)), aes(x=Var1, y=Var2, fill=value)) + labs(x="generations", y="activity") + 
  geom_tile() + scale_fill_gradientn(colours = colorRampPalette(c("white", "red", "blue"))(3), 
                                     values = c(0, 0.05 , 1), space = "Lab", guide = FALSE) + geom_hline(yintercept = 0)+ theme_bw() +
  theme(axis.title.x=element_text(size=16), axis.title.y=element_text(size=16), panel.grid.major = element_blank(), panel.grid.minor = element_blank(),axis.line = element_line(colour = "black"), legend.position = "none")

P_act

#ggsave(paste0(str1, ".png"), P_act)

### Competitiveness

str1 <- paste0(strID, "comp")
data <- read.table(paste0(str1, ".txt"), sep="\t", header = F)

df <- (t(subset(data, select = -c(V1, V1002))))
colnames(df) <- data$V1

length(df[1,])

minwv = 0.0;     # minimal (weight) value
maxwv = 2.0;     # maximal (weight) value
steps = 101;  # num. of bins across range
stepsize = (maxwv - minwv)/steps  # bin range size


#Frequency matrix:

mtrxwP1 <- matrix(nrow = steps, ncol = length(df[1,]), dimnames=list(seq(minwv+stepsize, maxwv, stepsize)))  # Frequency matrix
vecFpos <- vector()

for (i in 1:length(df[1,])){
  
  vecFpos[i] = length(which(df[,i] > 0.05))/length(df[,1])
  
  mtrxwP1[,i] = table(cut(df[,i], seq(minwv, maxwv, stepsize), right=T))/1000
  
}

colnames(mtrxwP1) <- data$V1


P_comp <- ggplot(data = melt(t(mtrxwP1)), aes(x=Var1, y=Var2, fill=value)) + labs(x="generations", y="competitiveness") + 
  geom_tile() + scale_fill_gradientn(colours = colorRampPalette(c("white", "red", "blue"))(3), 
                                     values = c(0, 0.05 , 1), space = "Lab", guide = FALSE) + geom_hline(yintercept = 0)+ theme_bw() +
  theme(axis.title.x=element_text(size=16), axis.title.y=element_text(size=16), panel.grid.major = element_blank(), panel.grid.minor = element_blank(),axis.line = element_line(colour = "black"), legend.position = "none")+
  scale_y_continuous(labels = scales::number_format(accuracy = 0.01))

P_comp 

#ggsave(paste0(str1, ".png"), P_comp)

### Boldness

str1 <- paste0(strID,"bold")

data <- read.table(paste0(str1, ".txt"), sep="\t", header = F)

df <- (t(subset(data, select = -c(V1, V1002))))
colnames(df) <- data$V1

length(df[1,])

minwv = 0.0;     # minimal (weight) value
maxwv = 1.0;     # maximal (weight) value
steps = 101;  # num. of bins across range
stepsize = (maxwv - minwv)/steps  # bin range size


#Frequency matrix:

mtrxwP1 <- matrix(nrow = steps, ncol = length(df[1,]), dimnames=list(seq(minwv+stepsize, maxwv, stepsize)))  # Frequency matrix
vecFpos <- vector()

for (i in 1:length(df[1,])){
  
  vecFpos[i] = length(which(df[,i] > 0.05))/length(df[,1])
  
  mtrxwP1[,i] = table(cut(df[,i], seq(minwv, maxwv, stepsize), right=T))/1000
  
}

colnames(mtrxwP1) <- data$V1


P_bold <- ggplot(data = melt(t(mtrxwP1)), aes(x=Var1, y=Var2, fill=value)) + labs(x="generations", y="boldness") + 
  geom_tile() + scale_fill_gradientn(colours = colorRampPalette(c("white", "red", "blue"))(3), 
                                     values = c(0, 0.05 , 1), space = "Lab", guide = FALSE) + geom_hline(yintercept = 0)+ theme_bw() +
  theme(axis.title.x=element_text(size=16), axis.title.y=element_text(size=16), panel.grid.major = element_blank(), panel.grid.minor = element_blank(),axis.line = element_line(colour = "black"), legend.position = "none")+
  scale_y_continuous(labels = scales::number_format(accuracy = 0.01))

P_bold

#ggsave(paste0(str1, ".png"), P_bold)
##############################################
library(ggpubr)
figure <- ggarrange(P_act+ rremove("x.text") + rremove("xlab"), P_comp+ rremove("x.text") + rremove("xlab"),
          P_bold, 
          labels = c("A", "B", "C"),
          ncol = 1, align = "v")

ggsave(paste0(strID, ".png"), figure, width = 29.7, height = 20.1, units = "cm")



#frequencies of morphs, custom
df_H = data.frame(data$V1, vecFpos, 1 - vecFpos)
colnames(df_H) <- c("gens", "highact", "lowact")

frequencies <- ggplot(data=df_H, aes(x=gens, y=highact, colour="weight > 0")) +
  geom_line(colour="darkgreen")  + theme(axis.title=element_text(size=12))+ 
  geom_line(data = df_H, aes(x = gens, y = lowact, colour="weight < 0"), colour="purple")  + 
  labs(x="", y="Herbivore frequency", colour="")+ theme(legend.position='none') +
  theme(panel.background = element_blank(), panel.border = element_rect(colour = "black", fill=NA),  
        axis.line = element_line(colour = "black"), axis.title.y = element_text(size = 9), axis.text=element_text(size=8))


#####
# IFD measures

idf_data <- read.table("IDF.txt", header = T)

plot(idf_data$G, idf_data$avg_ttifd)


######3.8.2021######

#data <- read.table(paste0(strID,"3000_landscape.txt"), sep="\t", header = T)
data <- read_delim(paste0(strID,"_landscape.txt"), delim="\t")

#Create cell and individual ID
data <- data %>% 
  mutate(cell = xpos * 10 + ypos,
         ind  = 1:nrow(data) %% 10000 )

# Summarize data at cell level
scapedata <- data %>% 
  group_by(scene, time, cell) %>%
  summarize(
    count = n(),
    sumcomp = sum(comp),
    sdcomp = sd(comp),
    sumact = sum(act),
    sdact = sd(act),
    resource = intake[1] * sumcomp / comp[1]
  )
#Add resource level of cell to "data"
data <- data %>%
  left_join(select(scapedata, scene, time, cell, resource), by=c("cell"= "cell", "scene" = "scene", "time" = "time"))
# borders need to be defined by hand (at least for now)

data <- mutate(data, morph = cut(comp, c(0.0, 0.15, 0.26, 0.4, 0.5, 0.75, 1.0, 2.0), labels = paste0("morph", 1:7)))
  
morphs <- data %>%
  group_by(scene, time, morph) %>%
  summarize(
    count = n(),
    sumint = sum(intake),
    average = sumint / count,
    meancomp = mean(comp)
)
  
  
  

#10% sample of full data
data.new = data[seq(1, nrow(data), 10), ]



# distribution of competitiveness across resource levels
ggplot(filter(data.new, scene == 1), aes(resource, comp))+
  geom_point(alpha = 0.05)+facet_wrap(~time)

#Individuals segregate, with highly competitive individuals assembling on 
#high resource patches. Higher variation for highly competitive individuals. 
#Also see this boxplot, separated into morphs:

complevels <- round(with(data, tapply(comp, morph, mean)), digits = 2)

ggplot(filter(data.new, scene == 1), aes(resource, morph))+
  geom_boxplot(alpha = 0.05)+facet_wrap(~time) + scale_y_discrete(labels = complevels)

######################

#Final food accumulated across different competitiveness values
ggplot(data = filter(data, scene == max(scene), time == max(time))) +geom_point(aes(x = comp, y = food))

#final fitness values (food corrected for costs)
ggplot(filter(data, scene == max(scene), time == max(time)), aes(morph, food - comp * 0.005 * 100 - act * 0.0001*100))+
  geom_violin()

#Population proportion
ggplot(filter(data, scene == max(scene), time == max(time)))+
         geom_bar(aes(x = morph))




#number of individuals on cells of different resource levels, 
#across different timesteps and scenes
ggplot(scapedata, aes(x = resource, y = count, colour = time))+
  geom_point()+facet_wrap(~scene)

#Cumulative competitveness value on cells of different resource levels, 
#across different timesteps and scenes
ggplot(scapedata, aes(x = resource, y = sumcomp, colour = time))+
  geom_point()+facet_wrap(~scene)

#SD of competitiveness on cells of different resource levels, 
#across different timesteps and scenes
ggplot(scapedata, aes(x = resource, y = sdcomp, colour = time))+
  geom_point()+facet_wrap(~scene)





##########
#analyse clusters

#Individual intake rates across time, with competitiveness values colored in
ggplot(data = filter(data, scene == 1)) +geom_jitter(aes(x = time, y = intake, colour = comp))+
  ylim(0,0.05)

#Average intake rate of the 7 morphs across scenes (facets) and timesteps (xaxis)
ggplot(morphs, aes(time, average, colour = morph))+
  geom_line()+facet_wrap(~scene)

#same without facet
ggplot(morphs, aes(x = time, y = average, group = interaction(morph, scene), colour = morph))+
  geom_line()

###Conclusions: WHat is happening?
# Polymorphisms are driven by the interplay between spatial assortment within scenes, 
# where resource level and individual competitiveness align, and
# the redistribution of resources, that benefits less competitive, 
# spatially assorted morphs. Since similar morphs co-occur, it pays for 
# intermediate morphs to stay more competitive than the adjacent less competitive morphs
# in case of a favorable habitat shift.






