package br.com.smartgrowtent.models;

import com.fasterxml.jackson.annotation.JsonProperty;
import jakarta.persistence.*;
import lombok.*;

import java.io.Serializable;
import java.time.LocalDateTime;

@Entity @Builder @Data
@AllArgsConstructor @NoArgsConstructor
@Table(name = "grow_values")
@EqualsAndHashCode(of = "dateTime")
public class GrowValues implements Serializable {

    @Id
    @Column(nullable = false)
    @JsonProperty("dateTime")
    private LocalDateTime dateTime;

    @Column(nullable = false)
    @JsonProperty("temperature")
    private Double temperature;

    @Column(nullable = false)
    @JsonProperty("humidity")
    private Double humidity;

}
