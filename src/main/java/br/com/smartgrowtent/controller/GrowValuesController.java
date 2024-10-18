package br.com.smartgrowtent.controller;

import br.com.smartgrowtent.models.GrowValues;
import br.com.smartgrowtent.repositories.GrowValuesRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.MediaType;
import org.springframework.web.bind.annotation.*;

import java.time.LocalDateTime;
import java.util.List;

@RestController
@ResponseBody
@RequestMapping("/smartgrowtent/api/grow-values")
public class GrowValuesController {

    @Autowired
    GrowValuesRepository repository;

    @GetMapping(produces = MediaType.APPLICATION_JSON_VALUE)
    public List<GrowValues> getGrowValues() {
        return repository.findAll();
    }

    @PostMapping(produces = MediaType.APPLICATION_JSON_VALUE, consumes = MediaType.APPLICATION_JSON_VALUE)
    public GrowValues postGrowValues(@RequestBody GrowValues growValues) {
        growValues.setDateTime(LocalDateTime.now());
        return repository.save(growValues);
    }
}
