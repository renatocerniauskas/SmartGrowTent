package br.com.smartgrowtent.domain;

import lombok.Builder;
import lombok.Data;

import java.time.LocalDateTime;

@Builder @Data
public class GrowValuesDomain {

    private Double temperature;
    private Double humidity;
    private LocalDateTime dateTime;
}
