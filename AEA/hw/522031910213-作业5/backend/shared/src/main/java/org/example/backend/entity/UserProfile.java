package org.example.backend.entity;


import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;


@AllArgsConstructor
@NoArgsConstructor
@Data
public class UserProfile {
    private String username;
    private String email;
    private String tel;
    private String aboutMe;
}
