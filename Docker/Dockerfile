
FROM openjdk:24-oraclelinux9
LABEL authors="SmartGrowTent"
LABEL maintainer="SmartGrowTent"
COPY /target/smartgrowtent-0.0.1-SNAPSHOT.jar /app/smartgrowtent-0.0.1-SNAPSHOT.jar
WORKDIR /app
ENTRYPOINT ["java", "-jar", "smartgrowtent-0.0.1-SNAPSHOT.jar"]
EXPOSE 8080