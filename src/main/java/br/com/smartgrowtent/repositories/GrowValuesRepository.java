package br.com.smartgrowtent.repositories;

import br.com.smartgrowtent.models.GrowValues;
import org.springframework.data.jpa.repository.JpaRepository;

import java.time.LocalDateTime;
import java.util.List;

public interface GrowValuesRepository extends JpaRepository<GrowValues,Long> {
    List<GrowValues> findByTimestampBetween(LocalDateTime start, LocalDateTime end);
}
