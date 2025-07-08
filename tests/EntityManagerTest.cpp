#include <gtest/gtest.h>
#include "EntityManager.hpp"
#include "Types.hpp" // Required for Entity and Signature types

TEST(EntityManagerTest, CreateAndDestroyEntities) {
    // ARRANGE
    EntityManager entityManager;

    // ACT
    Entity entity1 = entityManager.createEntity();
    Entity entity2 = entityManager.createEntity();

    // ASSERT
    ASSERT_NE(entity1, entity2);
    ASSERT_EQ(entityManager.getActiveEntityCount(), 2);

    entityManager.removeEntity(entity1);
    ASSERT_EQ(entityManager.getActiveEntityCount(), 1);

    Entity entity3 = entityManager.createEntity();
    ASSERT_EQ(entity3, entity1); // Assert the destroyed ID was reused
    ASSERT_EQ(entityManager.getActiveEntityCount(), 2);
}

TEST(EntityManagerTest, AssignAndGetSignature) {
    // ARRANGE
    EntityManager entityManager;
    Entity entity = entityManager.createEntity();

    // ACT & ASSERT: Check initial signature
    // An entity should be created with an empty signature.
    Signature initialSignature = entityManager.getSignature(entity);
    ASSERT_TRUE(initialSignature.none()); // .none() checks if all bits are 0

    // ACT & ASSERT: Assign a new signature
    Signature newSignature;
    newSignature.set(5);  // Set bit for a hypothetical component with ID 5
    newSignature.set(10); // Set bit for a hypothetical component with ID 10
    entityManager.assignSignature(entity, newSignature);

    Signature retrievedSignature = entityManager.getSignature(entity);
    ASSERT_EQ(retrievedSignature, newSignature);
    ASSERT_TRUE(retrievedSignature.test(5));
    ASSERT_TRUE(retrievedSignature.test(10));
    ASSERT_FALSE(retrievedSignature.test(1));
}