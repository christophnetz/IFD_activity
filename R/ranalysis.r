library(reshape2)
library(ggplot2)
library(scales)
library(readr)
library(tidyverse)
setwd("C:/Users/user/Desktop/IFDxpersonality/IFD_activity/IFD_activity")


strID <- "Evol16"
# 
# str1 <- paste0(strID,"activities")
# data <- read.table(paste0(str1, ".txt"), sep="\t", header = F)
# 
# df <- (t(subset(data, select = -c(V1, V1002))))
# colnames(df) <- data$V1
# 
# length(df[1,])
# 
# minwv = 0.0;     # minimal (weight) value
# maxwv = 2.0;     # maximal (weight) value
# steps = 101;  # num. of bins across range
# stepsize = (maxwv - minwv)/steps  # bin range size
# 
# 
# #Frequency matrix:
# 
# mtrxwP1 <- matrix(nrow = steps, ncol = length(df[1,]), dimnames=list(seq(minwv+stepsize, maxwv, stepsize)))  # Frequency matrix
# vecFpos <- vector()
# 
# for (i in 1:length(df[1,])){
#   
#   vecFpos[i] = length(which(df[,i] > 0.05))/length(df[,1])
#   
#   mtrxwP1[,i] = table(cut(df[,i], seq(minwv, maxwv, stepsize), right=T))/1000
#   
# }
# 
# colnames(mtrxwP1) <- data$V1
# 
# 
# P_act <- ggplot(data = melt(t(mtrxwP1)), aes(x=Var1, y=Var2, fill=value)) + labs(x="generations", y="activity") + 
#   geom_tile() + scale_fill_gradientn(colours = colorRampPalette(c("white", "red", "blue"))(3), 
#                                      values = c(0, 0.05 , 1), space = "Lab", guide = FALSE) + geom_hline(yintercept = 0)+ theme_bw() +
#   theme(axis.title.x=element_text(size=16), axis.title.y=element_text(size=16), panel.grid.major = element_blank(), panel.grid.minor = element_blank(),axis.line = element_line(colour = "black"), legend.position = "none")
# P_act

#ggsave(paste0(str1, ".png"), P_act)

### Competitiveness


str1 <- paste0(strID, "comp")
data <- read.table(paste0(str1, ".txt"), sep="\t", header = F)

df <- (t(subset(data, select = -c(V1, V1002))))
colnames(df) <- data$V1

length(df[1,])

minwv = 0.0;     # minimal (weight) value
maxwv = 1.2;     # maximal (weight) value
steps = 200;     # num. of bins across range
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

P_comp #+ ylim(0.40, 0.6)

ggsave(paste0(str1, ".png"), P_comp, width = 6.5)

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
         ind  = 1:nrow(data) %% 10000,
         timesincechange = time - lastchange
         
         )

# Summarize data at cell level
scapedata <- data %>% 
  group_by(time, cell) %>%
  summarize(
    count = n(),
    sumcomp = sum(comp),
    sdcomp = sd(comp),
    sumact = sum(act),
    sdact = sd(act),
    resource = intake[1] * sumcomp / comp[1]
    #timesincechange = time - lastchange
    
  )
#Add resource level of cell to "data"
data <- data %>%
  left_join(select(scapedata, time, cell, resource), by=c("cell"= "cell", "time" = "time"))
# borders need to be defined by hand (at least for now)

data <- mutate(data, morph = cut(comp, c(0.0, 0.6, 0.8, 1.0, 1.2, 2.0), labels = paste0("morph", 1:5)))
  
morphs <- data %>%
  group_by(time, morph) %>%
  summarize(
    count = n(),
    sumint = sum(intake),
    average = sumint / count,
    meancomp = mean(comp),
    timesincechange2 = mean(timesincechange)
)
  
  
  

#10% sample of full data
data.new = data[seq(1, nrow(data), 10), ]



# distribution of competitiveness across resource levels
ggplot(filter(data.new, time < 10), aes(resource, comp))+
  geom_point(alpha = 0.05)+facet_wrap(~time)

ggplot(filter(data.new, time == 3), aes(resource, comp))+
  geom_point(alpha = 0.05)+geom_smooth()


#Individuals segregate, with highly competitive individuals assembling on 
#high resource patches. Higher variation for highly competitive individuals. 
#Also see this boxplot, separated into morphs:

complevels <- round(with(data, tapply(comp, morph, mean)), digits = 2)

P_spat <- ggplot(filter(data.new, time < 9), aes(resource, morph, colour = morph))+
  geom_boxplot(alpha = 0.05)+facet_wrap(~time, ncol = 4) + scale_y_discrete(labels = complevels)+
  theme(legend.position = "none")

ggsave(paste0(str1, "_spat.png"), P_spat, width = 6.5)

######################

#Final food accumulated across different competitiveness values
ggplot(data = filter(data, time == max(time))) +geom_point(aes(x = comp, y = food))

#final fitness values (food corrected for costs)
ggplot(filter(data, time == max(time)), aes(morph, food - comp * 0.005 * 200))+
  geom_violin()+ylab("fitness")

#Population proportion
ggplot(filter(data, time == max(time)))+
         geom_bar(aes(x = morph))




#number of individuals on cells of different resource levels, 
#across different timesteps and scenes
ggplot(filter(scapedata, time < 5), aes(x = resource, y = count, colour = time))+
  geom_point()
ggplot(filter(scapedata, time < 5), aes(x = resource, y = count))+
  geom_point()+facet_wrap(~time)+labs(y="individuals")


#Cumulative competitveness value on cells of different resource levels, 
#across different timesteps and scenes
ggplot(filter(scapedata, time<5), aes(x = resource, y = sumcomp, colour = time))+
  geom_point()

ggplot(filter(scapedata, time < 5), aes(x = resource, y = sumcomp))+
  geom_point()+facet_wrap(~time)+labs(y="competitiveness on patch")

plotRB <- ggplot(filter(scapedata, time < 5), aes(x = resource, y = sumcomp))+
  geom_point()+geom_point(aes(x = resource, y = count), colour="red")+facet_wrap(~time)+
  labs(y="individuals || competitiveness")

ggplot(filter(scapedata, time < 5), aes(x = resource, y = sumcomp/count))+
  geom_point()+facet_wrap(~time)+
  labs(y="avg. competitiveness")

ggplot(filter(scapedata, time < 5), aes(x = resource, y = resource/sumcomp))+
  geom_point()+facet_wrap(~time)+
  labs(y="avg. competitiveness")

ggsave(paste0(str1, "_RB.png"), plotRB, width = 6.5)


#SD of competitiveness on cells of different resource levels, 
#across different timesteps and scenes
ggplot(scapedata, aes(x = resource, y = sdcomp, colour = time))+
  geom_point()+facet_wrap(~scene)




##########
#analyse clusters

#Individual intake rates across time, with competitiveness values colored in
ggplot(data = filter(data, time < 11)) +geom_jitter(aes(x = time, y = intake, colour = comp))+
  ylim(0,0.05)

#Average intake rate of the 7 morphs across scenes (facets) and timesteps (xaxis)
ggplot(morphs, aes(time, average, colour = morph))+
  geom_line()+facet_wrap(~time)

#same without facet
ggplot(filter(morphs, time < 18), aes(x = time, y = average, group = morph, colour = morph))+
  geom_line()

intakeplot <- ggplot(morphs, aes(x = timesincechange2 -1, y = average, group = meancomp, colour = as.factor(round(meancomp, digits = 2))))+
  geom_smooth(se = FALSE)+ geom_point( alpha = 1/5, shape = 16)+labs(x="time since change", y = "average intake") + 
  guides(colour=guide_legend(title="competitive ability"))

ggsave(paste0(strID, "_intake.png"), intakeplot, width = 6.5)


###Conclusions: What is happening?
# Polymorphisms are driven by the interplay between spatial assortment within scenes, 
# where resource level and individual competitiveness align, and
# the redistribution of resources, that benefits less competitive, 
# spatially assorted morphs. Since similar morphs co-occur, it pays for 
# intermediate morphs to stay more competitive than the adjacent less competitive morphs
# in case of a favorable habitat shift.







