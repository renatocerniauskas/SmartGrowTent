package br.com.smartgrowtent.controller;


import br.com.smartgrowtent.models.GrowValues;
import br.com.smartgrowtent.repositories.GrowValuesRepository;
import br.com.smartgrowtent.service.GrowValuesService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.time.LocalDateTime;
import java.util.Arrays;
import java.util.List;
import java.util.Optional;

@RestController
@RequestMapping("/api/chart/")
public class ApiChartController {

    @Autowired
    private GrowValuesService service;

    @GetMapping
    public List<GrowValues> getChartData(@RequestParam("start") String start, @RequestParam("end") String end) {
        return service.getGrowValues(LocalDateTime.parse(start), LocalDateTime.parse(end));
    }

    @GetMapping("last-hours")
    public List<GrowValues> getChartDataLastHours(@RequestParam("hours") int hours) {
        return service.getGrowValuesLastHours(hours);
    }

    @GetMapping("last")
    public Optional<GrowValues> getChartDataLast() {
        return service.getGrowValuesLast();
    }
}
