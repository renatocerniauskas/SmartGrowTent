package br.com.smartgrowtent.controller;

import br.com.smartgrowtent.models.GrowValues;
import br.com.smartgrowtent.repositories.GrowValuesRepository;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import java.time.LocalDateTime;
import java.util.List;

@Slf4j
@RestController
@ResponseBody
@RequestMapping("/smartgrowtent/api/grow-values")
public class GrowValuesController {

    @Autowired
    GrowValuesRepository repository;

    @GetMapping
    public ResponseEntity<List<GrowValues>> getGrowValues() {
        return ResponseEntity.status(HttpStatus.FOUND).body(repository.findAll());
    }

    @PostMapping
    public ResponseEntity<GrowValues> postGrowValues(@RequestParam Double temperature, Double humidity,
                                                     Double eco2, Double etvoc) {
        LocalDateTime localDateTime = LocalDateTime.now();
        log.info("gravando dado na tabela: timestamp={} teperatura={} humidade={} eco2={} etvoc={}", localDateTime, temperature, humidity, eco2, etvoc);
        return ResponseEntity.status(HttpStatus.CREATED).body(
                repository.save(GrowValues.builder()
                        .temperature(temperature)
                        .humidity(humidity)
                        .timestamp(localDateTime)
                        .eco2(eco2)
                        .etvoc(etvoc)
                        .build())
        );
    }
}
