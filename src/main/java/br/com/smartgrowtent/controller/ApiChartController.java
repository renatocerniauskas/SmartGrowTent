package br.com.smartgrowtent.controller;


import br.com.smartgrowtent.models.GrowValues;
import br.com.smartgrowtent.repositories.GrowValuesRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.Arrays;
import java.util.List;

@RestController
public class ApiChartController {

    @Autowired
    GrowValuesRepository repository;



    @GetMapping("/api/chart")
    public List<Object[]> getChartData() {
        // Dados de exemplo
        List<Object[]> chartData = Arrays.asList(
                new Object[]{"Categoria", "Valor"},
                new Object[]{"A", 30},
                new Object[]{"B", 70},
                new Object[]{"C", 50}
        );
        return chartData;
    }

    @GetMapping("/api/chart2")
    public List<Object[]> getChartData2() {
        // Suponha que você tenha um serviço que retorna os dados do gráfico
        List<GrowValues> growValues = repository.findAll();

        List<Object[]> chartData = Arrays.asList(
                new Object[]{"Categoria", "Valor"},
                new Object[]{growValues.getFirst().getTemperature(), growValues.getFirst().getHumidity()}
        );
        return chartData;
    }

}
