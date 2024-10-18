package br.com.smartgrowtent.controller;

import br.com.smartgrowtent.models.GrowValues;
import br.com.smartgrowtent.repositories.GrowValuesRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.*;

import java.time.LocalDateTime;
import java.util.Arrays;
import java.util.List;

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
    public ResponseEntity<GrowValues> postGrowValues(@RequestBody GrowValues growValues) {
        growValues.setDateTime(LocalDateTime.now());
        return ResponseEntity.status(HttpStatus.CREATED).body(
                repository.save(growValues)
        );
    }
}
