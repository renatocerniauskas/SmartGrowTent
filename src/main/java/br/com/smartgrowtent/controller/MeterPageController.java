package br.com.smartgrowtent.controller;

import br.com.smartgrowtent.models.GrowValues;
import br.com.smartgrowtent.repositories.GrowValuesRepository;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;

@Controller
@Slf4j
public class MeterPageController {

    @Autowired
    GrowValuesRepository repository;

    @GetMapping("/meter")
    public String showGrow(Model model) {
        GrowValues lastValue = repository.findTopByOrderByTimestampDesc();
        return "meter";
    }

    private double calculateVPD(double temperature, double humidity) {
        double Es = 0.6108 * Math.exp((17.27 * temperature) / (temperature + 237.3));
        double Ea = Es * (humidity / 100);
        return Es - Ea;
    }
}
